#include "vm.hpp"
#include "shared_state.hpp"
#include "config_parser.hpp"
#include "config.h"
#include "objectmemory.hpp"
#include "instruments/tooling.hpp"
#include "instruments/timing.hpp"
#include "global_cache.hpp"
#include "capi/handle.hpp"

#include "util/thread.hpp"
#include "inline_cache.hpp"
#include "configuration.hpp"

#include "agent.hpp"
#include "world_state.hpp"

namespace rubinius {

  SharedState::SharedState(Environment* env, Configuration& config, ConfigParser& cp)
    : initialized_(false)
    , signal_handler_(0)
    , global_handles_(new capi::Handles)
    , cached_handles_(new capi::Handles)
    , global_serial_(0)
    , world_(new WorldState)
    , ic_registry_(new InlineCacheRegistry)
    , class_count_(0)
    , thread_ids_(0)
    , agent_(0)
    , root_vm_(0)
    , env_(env)
    , tool_broker_(new tooling::ToolBroker)
    , ruby_critical_set_(false)

    , om(0)
    , global_cache(new GlobalCache)
    , config(config)
    , user_variables(cp)
    , llvm_state(0)
  {
    ref();

    for(int i = 0; i < Primitives::cTotalPrimitives; i++) {
      primitive_hits_[i] = 0;
    }
  }

  SharedState::~SharedState() {
    if(!initialized_) return;

    // std::cerr << "Time waiting: " << world_->time_waiting() << "\n";
    delete world_;
    delete ic_registry_;
    delete om;
    delete global_cache;
    delete global_handles_;
    delete cached_handles_;
    if(agent_) {
      delete agent_;
    }
  }

  void SharedState::add_managed_thread(ManagedThread* thr) {
    SYNC(0);
    threads_.push_back(thr);
  }

  void SharedState::remove_managed_thread(ManagedThread* thr) {
    SYNC(0);
    threads_.remove(thr);
  }

  int SharedState::size() {
    return sizeof(SharedState) +
      sizeof(WorldState) +
      symbols.byte_size();
  }

  void SharedState::discard(SharedState* ss) {
    if(ss->deref()) delete ss;
  }

  uint32_t SharedState::new_thread_id(THREAD) {
    SYNC(state);
    return ++thread_ids_;
  }

  VM* SharedState::new_vm() {
    uint32_t id = new_thread_id(0);

    SYNC(0);

    // TODO calculate the thread id by finding holes in the
    // field of ids, so we reuse ids.

    VM* vm = new VM(id, *this);
    threads_.push_back(vm);

    this->ref();

    // If there is no root vm, then the first one created becomes it.
    if(!root_vm_) root_vm_ = vm;
    return vm;
  }

  void SharedState::remove_vm(VM* vm) {
    SYNC(0);
    this->deref();

    // Don't delete ourself here, it's too problematic.
  }


  void SharedState::add_global_handle(STATE, capi::Handle* handle) {
    SYNC(state);
    global_handles_->add(handle);
  }

  void SharedState::make_handle_cached(STATE, capi::Handle* handle) {
    SYNC(state);
    global_handles_->move(handle, cached_handles_);
  }


  QueryAgent* SharedState::autostart_agent(STATE) {
    SYNC(state);
    if(agent_) return agent_;
    agent_ = new QueryAgent(*this, root_vm_);
    return agent_;
  }

  /**
   * Create the preemption thread and call scheduler_loop() in the new thread.
   */
  void SharedState::enable_preemption() {
    interrupts.enable_preempt = true;
  }

  void SharedState::pre_exec() {
    SYNC(0);
    if(agent_) agent_->cleanup();
  }

  void SharedState::reinit() {
    // For now, we disable inline debugging here. This makes inspecting
    // it much less confusing.

    config.jit_inline_debug.set("no");

    world_->reinit();

    if(agent_) {
      agent_->on_fork();
      delete agent_;
      agent_ = 0;
    }
  }

  void SharedState::ask_for_stopage() {
    world_->ask_for_stopage();
  }

  bool SharedState::should_stop() {
    return world_->should_stop();
  }

  void SharedState::stop_the_world(THREAD) {
    world_->wait_til_alone(state);

    // Verify that everyone is stopped and we're alone.
    for(std::list<ManagedThread*>::iterator i = threads_.begin();
        i != threads_.end();
        i++) {
      ManagedThread *th = *i;
      switch(th->run_state()) {
      case ManagedThread::eAlone:
        if(th != state) {
          rubinius::bug("Tried to stop but someone else is alone!");
        }
        break;
      case ManagedThread::eRunning:
        rubinius::bug("Tried to stop but threads still running!");
        break;
      case ManagedThread::eSuspended:
      case ManagedThread::eIndependent:
        // Ok, this is fine.
        break;
      }
    }
  }

  void SharedState::restart_world(THREAD) {
    world_->wake_all_waiters(state);
  }

  bool SharedState::checkpoint(THREAD) {
    return world_->checkpoint(state);
  }

  void SharedState::gc_dependent(THREAD) {
    world_->become_dependent(state);
  }

  void SharedState::gc_independent(THREAD) {
    world_->become_independent(state);
  }

  void SharedState::set_critical(STATE) {
    SYNC(state);

    if(!ruby_critical_set_ ||
         !pthread_equal(ruby_critical_thread_, pthread_self())) {

      UNSYNC;
      GCIndependent gc_guard(state);
      ruby_critical_lock_.lock();
      ruby_critical_thread_ = pthread_self();
      ruby_critical_set_ = true;
    }

    return;
  }

  void SharedState::clear_critical(STATE) {
    SYNC(state);

    if(pthread_equal(ruby_critical_thread_, pthread_self())) {
      ruby_critical_set_ = false;
      ruby_critical_lock_.unlock();
    }
  }

  void SharedState::enter_capi(STATE) {
    capi_lock_.lock(state);
  }

  void SharedState::leave_capi(STATE) {
    capi_lock_.unlock(state);
  }
}
