#include "shared_state.hpp"
#include "config_parser.hpp"
#include "config.h"
#include "objectmemory.hpp"
#include "instruments/profiler.hpp"
#include "global_cache.hpp"
#include "capi/handle.hpp"

#include "util/thread.hpp"
#include "inline_cache.hpp"

namespace rubinius {

  class WorldState {
    thread::Mutex mutex_;
    thread::Condition waiting_to_stop_;
    thread::Condition waiting_to_run_;
    int pending_threads_;
    bool should_stop_;

  public:
    WorldState()
      : pending_threads_(0)
      , should_stop_(false)
    {}

    // Called after a fork(), when we know we're alone again, to get
    // everything back in the proper order.
    void reinit() {
      mutex_.init();
      waiting_to_stop_.init();
      waiting_to_run_.init();
      pending_threads_ = 1;
      should_stop_ = false;
    }

    // If called when the GC is waiting to run,
    //   wait until the GC tells us it's ok to continue.
    // always increments pending_threads_ at the end.
    void become_independent() {
      thread::Mutex::LockGuard guard(mutex_);

      // If someone is waiting on us to stop, stop now.
      if(should_stop_) wait_to_run();
      pending_threads_--;
    }

    void become_dependent() {
      thread::Mutex::LockGuard guard(mutex_);

      // If the GC is running, wait here...
      while(should_stop_) {
        waiting_to_run_.wait(mutex_);
      }

      pending_threads_++;
    }

    void wait_til_alone() {
      thread::Mutex::LockGuard guard(mutex_);
      should_stop_ = true;

      // For ourself..
      pending_threads_--;

      while(pending_threads_ > 0) {
        waiting_to_stop_.wait(mutex_);
      }
    }

    void wake_all_waiters() {
      thread::Mutex::LockGuard guard(mutex_);

      should_stop_ = false;

      // For ourself..
      pending_threads_++;

      waiting_to_run_.broadcast();
    }

    void checkpoint() {
      // Test should_stop_ without the lock, because we do this a lot.
      if(should_stop_) {
        thread::Mutex::LockGuard guard(mutex_);
        wait_to_run();
      }
    }

  private:
    void wait_to_run() {
      pending_threads_--;
      waiting_to_stop_.signal();

      while(should_stop_) {
        waiting_to_run_.wait(mutex_);
      }

      pending_threads_++;
    }
  };

  SharedState::SharedState(Configuration& config, ConfigParser& cp)
    : initialized_(false)
    , signal_handler_(0)
    , global_handles_(new capi::Handles)
    , profiling_(false)
    , profiler_collection_(0)
    , global_serial_(0)
    , world_(*new WorldState)
    , ic_registry_(new InlineCacheRegistry)
    , om(0)
    , global_cache(0)
    , config(config)
    , user_variables(cp)
    , llvm_state(0)
  {
    ref();
  }

  SharedState::~SharedState() {
    if(!initialized_) return;

    delete ic_registry_;
    delete om;
    delete global_cache;
    delete global_handles_;
  }

  void SharedState::discard(SharedState* ss) {
    if(ss->deref()) delete ss;
  }

  VM* SharedState::new_vm() {
    VM* vm = new VM(*this);
    cf_locations_.push_back(vm->call_frame_location());
    this->ref();
    return vm;
  }

  void SharedState::remove_vm(VM* vm) {
    cf_locations_.remove(vm->call_frame_location());
    this->deref();

    // Don't delete ourself here, it's too problematic.
  }

  void SharedState::enable_profiling(VM* vm) {
    profiler_collection_ = new profiler::ProfilerCollection(vm);
    profiling_ = true;
  }

  LookupTable* SharedState::disable_profiling(VM* vm) {
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
    if(profiler_collection_) {
      profiler_collection_->add_profiler(vm, profiler);
    }
  }

  void SharedState::remove_profiler(VM* vm, profiler::Profiler* profiler) {
    if(profiler_collection_) {
      profiler_collection_->remove_profiler(vm, profiler);
    }
  }

  void SharedState::reinit() {
    world_.reinit();
  }

  void SharedState::stop_the_world() {
    world_.wait_til_alone();
  }

  void SharedState::restart_world() {
    world_.wake_all_waiters();
  }

  void SharedState::checkpoint() {
    world_.checkpoint();
  }

  void SharedState::gc_dependent() {
    world_.become_dependent();
  }

  void SharedState::gc_independent() {
    world_.become_independent();
  }
}
