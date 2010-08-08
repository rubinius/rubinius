#include "vm.hpp"
#include "shared_state.hpp"
#include "config_parser.hpp"
#include "config.h"
#include "objectmemory.hpp"
#include "instruments/profiler.hpp"
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
    , profiling_(false)
    , profiler_collection_(0)
    , global_serial_(0)
    , world_(new WorldState)
    , ic_registry_(new InlineCacheRegistry)
    , class_count_(0)
    , timer_thread_started_(false)
    , agent_(0)
    , root_vm_(0)
    , env_(env)
    , thread_ids_(0)

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

    // If there is no root vm, then the first on created becomes it.
    if(!root_vm_) root_vm_ = vm;
    return vm;
  }

  void SharedState::remove_vm(VM* vm) {
    SYNC(0);
    threads_.remove(vm);
    this->deref();

    // Don't delete ourself here, it's too problematic.
  }

  QueryAgent* SharedState::autostart_agent() {
    SYNC(0);
    if(agent_) return agent_;
    agent_ = new QueryAgent(*this, root_vm_);
    return agent_;
  }

  void SharedState::enable_profiling(VM* vm) {
    SYNC(0);
    profiler_collection_ = new profiler::ProfilerCollection(vm);
    profiling_ = true;
  }

  LookupTable* SharedState::disable_profiling(VM* vm) {
    SYNC(0);
    if(profiler_collection_) {
      LookupTable* profile = profiler_collection_->results(vm);
      delete profiler_collection_;
      profiler_collection_ = 0;
      profiling_ = false;
      return profile;
    } else {
      return reinterpret_cast<LookupTable*>(Qnil);
    }
  }

  void SharedState::add_profiler(VM* vm, profiler::Profiler* profiler) {
    SYNC(0);
    if(profiler_collection_) {
      profiler_collection_->add_profiler(vm, profiler);
    }
  }

  void SharedState::remove_profiler(VM* vm, profiler::Profiler* profiler) {
    SYNC(0);
    if(profiler_collection_) {
      profiler_collection_->remove_profiler(vm, profiler);
    }
  }

  // Trampoline to call scheduler_loop()
  static void* __thread_tramp__(void* arg) {
    SharedState* ss = static_cast<SharedState*>(arg);
    ss->scheduler_loop();
    return NULL;
  }

  // Runs forever, telling the VM to reschedule threads ever 10 milliseconds
  void SharedState::scheduler_loop() {
    // First off, we don't want this thread ever receiving a signal.
    sigset_t mask;
    sigfillset(&mask);
    if(pthread_sigmask(SIG_SETMASK, &mask, NULL) != 0) {
      abort();
    }

    struct timespec requested;
    struct timespec actual;

    requested.tv_sec = 0;
    requested.tv_nsec = 10000000; // 10 milliseconds

    Interrupts& ints = interrupts;

    for(;;) {
      nanosleep(&requested, &actual);
      if(ints.enable_preempt) {
        ints.set_timer();
      }
    }
  }

  // Create the preemption thread and call scheduler_loop() in the new thread
  void SharedState::enable_preemption() {
    interrupts.enable_preempt = true;
    return;

    if(timer_thread_started_) return;

    timer_thread_started_ = true;

    if(pthread_create(&timer_thread_, NULL, __thread_tramp__, this) != 0) {
      std::cerr << "Unable to create timer thread!\n";
      exit(1);
    }
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
        assert(th == state && "Someone else is alone!");
        break;
      case ManagedThread::eRunning:
        assert(0 && "Thread still running");
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

    if(ruby_critical_thread_ == 0 ||
         ruby_critical_thread_ != pthread_self()) {

      UNSYNC;
      GCIndependent gc_guard(state);
      ruby_critical_lock_.lock();
      ruby_critical_thread_ = pthread_self();
    }

    return;
  }

  void SharedState::clear_critical(STATE) {
    SYNC(state);

    if(ruby_critical_thread_ == pthread_self()) {
      ruby_critical_lock_.unlock();
    }
  }
}
