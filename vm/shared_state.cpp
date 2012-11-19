#include "vm.hpp"
#include "shared_state.hpp"
#include "config_parser.hpp"
#include "config.h"
#include "objectmemory.hpp"
#include "environment.hpp"
#include "instruments/tooling.hpp"
#include "instruments/timing.hpp"
#include "global_cache.hpp"
#include "capi/handles.hpp"
#include "capi/tag.hpp"

#include "util/thread.hpp"
#include "inline_cache.hpp"
#include "configuration.hpp"

#include "agent.hpp"
#include "world_state.hpp"
#include "builtin/randomizer.hpp"
#include "builtin/array.hpp"
#include "builtin/thread.hpp"

#include <iostream>
#include <iomanip>

#ifdef ENABLE_LLVM
#include "llvm/state.hpp"
#endif

namespace rubinius {

  SharedState::SharedState(Environment* env, Configuration& config, ConfigParser& cp)
    : initialized_(false)
    , auxiliary_threads_(0)
    , signal_handler_(0)
    , global_handles_(new capi::Handles)
    , global_serial_(0)
    , world_(new WorldState)
    , ic_registry_(new InlineCacheRegistry)
    , class_count_(0)
    , thread_ids_(1)
    , kcode_page_(kcode::eAscii)
    , kcode_table_(kcode::null_table())
    , agent_(0)
    , root_vm_(0)
    , env_(env)
    , tool_broker_(new tooling::ToolBroker)
    , ruby_critical_set_(false)
    , use_capi_lock_(false)
    , check_gc_(false)
    , om(0)

    , global_cache(new GlobalCache)
    , config(config)
    , user_variables(cp)
    , llvm_state(0)
  {
    ref();

    auxiliary_threads_ = new AuxiliaryThreads();

    for(int i = 0; i < Primitives::cTotalPrimitives; i++) {
      primitive_hits_[i] = 0;
    }

    hash_seed = Randomizer::random_uint32();
  }

  SharedState::~SharedState() {
    if(!initialized_) return;

    if(config.gc_show) {
      std::cerr << "Time spent waiting for the world to stop: " << world_->time_waiting() << "ns\n";
    }

#ifdef ENABLE_LLVM
    if(llvm_state) {
      delete llvm_state;
    }
#endif

    if(agent_) {
      delete agent_;
    }

    delete global_handles_;
    delete tool_broker_;
    delete global_cache;
    delete ic_registry_;
    delete world_;
    delete om;
    delete auxiliary_threads_;
  }

  void SharedState::add_managed_thread(ManagedThread* thr) {
    SYNC_TL;
    threads_.push_back(thr);
  }

  void SharedState::remove_managed_thread(ManagedThread* thr) {
    SYNC_TL;
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

  uint32_t SharedState::new_thread_id() {
    return atomic::fetch_and_add(&thread_ids_, 1);
  }

  VM* SharedState::new_vm() {
    uint32_t id = new_thread_id();

    SYNC_TL;

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
    SYNC_TL;
    threads_.remove(vm);
    this->deref();

    // Don't delete ourself here, it's too problematic.
  }

  Array* SharedState::vm_threads(STATE) {
    SYNC_TL;

    Array* threads = Array::create(state, 0);
    for(std::list<ManagedThread*>::iterator i = threads_.begin();
        i != threads_.end();
        ++i) {
      if(VM* vm = (*i)->as_vm()) {
        Thread *thread = vm->thread.get();
        if(!thread->system_thread() && CBOOL(thread->alive())) {
          threads->append(state, thread);
        }
      }
    }
    return threads;
  }

  capi::Handle* SharedState::add_global_handle(STATE, Object* obj) {
    if(!obj->reference_p()) {
      rubinius::bug("Trying to add a handle for a non reference");
    }
    uintptr_t handle_index = global_handles_->allocate_index(state, obj);
    obj->set_handle_index(state, handle_index);
    return obj->handle(state);
  }

  void SharedState::make_handle_cached(STATE, capi::Handle* handle) {
    SYNC(state);
    cached_handles_.push_back(handle);
  }

  void SharedState::add_global_handle_location(capi::Handle** loc,
                                               const char* file, int line)
  {
    SYNC_TL;
    if(*loc && REFERENCE_P(*loc)) {
      if(!global_handles_->validate(*loc)) {
        std::cerr << std::endl << "==================================== ERROR ====================================" << std::endl;
        std::cerr << "| An extension is trying to add an invalid handle at the following location:  |" << std::endl;
        std::ostringstream out;
        out << file << ":" << line;
        std::cerr << "| " << std::left << std::setw(75) << out.str() << " |" << std::endl;
        std::cerr << "|                                                                             |" << std::endl;
        std::cerr << "| An invalid handle means that it points to an invalid VALUE. This can happen |" << std::endl;
        std::cerr << "| when you haven't initialized the VALUE pointer yet, in which case we        |" << std::endl;
        std::cerr << "| suggest either initializing it properly or otherwise first initialize it to |" << std::endl;
        std::cerr << "| NULL if you can only set it to a proper VALUE pointer afterwards. Consider  |" << std::endl;
        std::cerr << "| the following example that could cause this problem:                        |" << std::endl;
        std::cerr << "|                                                                             |" << std::endl;
        std::cerr << "| VALUE ptr;                                                                  |" << std::endl;
        std::cerr << "| rb_gc_register_address(&ptr);                                               |" << std::endl;
        std::cerr << "| ptr = rb_str_new(\"test\");                                                   |" << std::endl;
        std::cerr << "|                                                                             |" << std::endl;
        std::cerr << "| Either change this register after initializing                              |" << std::endl;
        std::cerr << "|                                                                             |" << std::endl;
        std::cerr << "| VALUE ptr;                                                                  |" << std::endl;
        std::cerr << "| ptr = rb_str_new(\"test\");                                                   |" << std::endl;
        std::cerr << "| rb_gc_register_address(&ptr);                                               |" << std::endl;
        std::cerr << "|                                                                             |" << std::endl;
        std::cerr << "| Or initialize it with NULL:                                                 |" << std::endl;
        std::cerr << "|                                                                             |" << std::endl;
        std::cerr << "| VALUE ptr = NULL;                                                           |" << std::endl;
        std::cerr << "| rb_gc_register_address(&ptr);                                               |" << std::endl;
        std::cerr << "| ptr = rb_str_new(\"test\");                                                   |" << std::endl;
        std::cerr << "|                                                                             |" << std::endl;
        std::cerr << "================================== ERROR ======================================" << std::endl;
        rubinius::bug("Halting due to invalid handle");
      }
    }

    capi::GlobalHandle* global_handle = new capi::GlobalHandle(loc, file, line);
    global_handle_locations_.push_back(global_handle);
  }

  void SharedState::del_global_handle_location(capi::Handle** loc) {
    SYNC_TL;

    for(std::list<capi::GlobalHandle*>::iterator i = global_handle_locations_.begin();
        i != global_handle_locations_.end(); ++i) {
      if((*i)->handle() == loc) {
        delete *i;
        global_handle_locations_.erase(i);
        return;
      }
    }
    rubinius::bug("Removing handle not in the list");
  }

  QueryAgent* SharedState::start_agent(STATE) {
    SYNC(state);

    if(!agent_) {
      agent_ = new QueryAgent(state);
    }

    return agent_;
  }

  void SharedState::reinit(STATE) {
    // For now, we disable inline debugging here. This makes inspecting
    // it much less confusing.

    config.jit_inline_debug.set("no");

    env_->set_root_vm(state->vm());
    threads_.clear();
    threads_.push_back(state->vm());

    // Reinit the locks for this object
    lock_init(state->vm());
    global_cache->lock_init(state->vm());
    ic_registry_->lock_init(state->vm());
    onig_lock_.init();
    ruby_critical_lock_.init();
    capi_lock_.init();

    world_->reinit();
  }

  bool SharedState::should_stop() {
    return world_->should_stop();
  }

  bool SharedState::stop_the_world(THREAD) {
    return world_->wait_til_alone(state);
  }

  void SharedState::stop_threads_externally() {
    world_->stop_threads_externally();
  }

  void SharedState::restart_world(THREAD) {
    world_->wake_all_waiters(state);
  }

  void SharedState::restart_threads_externally() {
    world_->restart_threads_externally();
  }

  bool SharedState::checkpoint(THREAD) {
    return world_->checkpoint(state);
  }

  void SharedState::gc_dependent(STATE) {
    world_->become_dependent(state->vm());
  }

  void SharedState::gc_independent(STATE) {
    world_->become_independent(state->vm());
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

    if(ruby_critical_set_ && pthread_equal(ruby_critical_thread_, pthread_self())) {
      ruby_critical_set_ = false;
      ruby_critical_lock_.unlock();
    }
  }

  void SharedState::enter_capi(STATE, const char* file, int line) {
    if(use_capi_lock_) {
      capi_lock_.lock(state->vm(), file, line);
    }
  }

  void SharedState::leave_capi(STATE) {
    if(use_capi_lock_) {
      capi_lock_.unlock(state->vm());
    }
  }
}
