#include "config.h"
#include "vm.hpp"
#include "immix_marker.hpp"

#include "builtin/class.hpp"
#include "builtin/thread.hpp"
#include "configuration.hpp"
#include "gc/gc.hpp"
#include "gc/immix.hpp"
#include "ontology.hpp"

#include "dtrace/dtrace.h"
#include "instruments/timing.hpp"

namespace rubinius {

  Object* immix_marker_tramp(STATE) {
    state->memory()->immix_marker()->perform(state);
    return cNil;
  }

  ImmixMarker::ImmixMarker(STATE, ImmixGC* immix)
    : AuxiliaryThread()
    , shared_(state->shared())
    , self_(NULL)
    , immix_(immix)
    , data_(NULL)
    , paused_(false)
    , exit_(false)
    , thread_(state)
  {
    shared_.auxiliary_threads()->register_thread(this);
    state->memory()->set_immix_marker(this);

    run_lock_.init();
    run_cond_.init();
    pause_cond_.init();

    start_thread(state);
  }

  ImmixMarker::~ImmixMarker() {
    shared_.auxiliary_threads()->unregister_thread(this);
  }

  void ImmixMarker::start_thread(STATE) {
    SYNC(state);
    if(self_) return;
    utilities::thread::Mutex::LockGuard lg(run_lock_);
    self_ = state->shared().new_vm();
    paused_ = false;
    exit_ = false;
    thread_.set(Thread::create(state, self_, G(thread), immix_marker_tramp, false, true));
    run(state);
  }

  void ImmixMarker::stop_thread(STATE) {
    SYNC(state);
    if(!self_) return;

    pthread_t os = self_->os_thread();
    {
      utilities::thread::Mutex::LockGuard lg(run_lock_);
      // Thread might have already been stopped
      exit_ = true;
      run_cond_.signal();
    }

    void* return_value;
    pthread_join(os, &return_value);
    self_ = NULL;
  }

  void ImmixMarker::shutdown(STATE) {
    stop_thread(state);
  }

  void ImmixMarker::before_exec(STATE) {
    stop_thread(state);
  }

  void ImmixMarker::after_exec(STATE) {
    start_thread(state);
  }

  void ImmixMarker::before_fork(STATE) {
    utilities::thread::Mutex::LockGuard lg(run_lock_);
    while(!paused_ && self_->run_state() == ManagedThread::eRunning) {
      pause_cond_.wait(run_lock_);
    }
  }

  void ImmixMarker::after_fork_parent(STATE) {
    utilities::thread::Mutex::LockGuard lg(run_lock_);
    run_cond_.signal();
  }

  void ImmixMarker::after_fork_child(STATE) {
    run_lock_.init();
    run_cond_.init();
    pause_cond_.init();

    if(self_) {
      VM::discard(state, self_);
      self_ = NULL;
    }

    if(data_) {
      delete data_;
      data_ = NULL;
    }

    start_thread(state);
  }

  void ImmixMarker::concurrent_mark(GCData* data) {
    utilities::thread::Mutex::LockGuard lg(run_lock_);
    paused_ = false;
    data_ = data;
    run_cond_.signal();
  }

  void ImmixMarker::wait_for_marker(STATE) {
    utilities::thread::Mutex::LockGuard lg(run_lock_);
    GCTokenImpl gct;
    while(!paused_) {
      state->gc_independent(gct, state->vm()->saved_call_frame());
      pause_cond_.wait(run_lock_);
      state->gc_dependent(gct, state->vm()->saved_call_frame());
    }
  }

  void ImmixMarker::run(STATE) {
    int error = thread_.get()->fork_attached(state);
    if(error) rubinius::bug("Unable to immix marker thread");
  }

  void ImmixMarker::perform(STATE) {
    GCTokenImpl gct;
    const char* thread_name = "rbx.immix";
    self_->set_name(thread_name);

    RUBINIUS_THREAD_START(thread_name, state->vm()->thread_id(), 1);

    state->vm()->thread->hard_unlock(state, gct, 0);

    while(!exit_) {
      if(data_) {
        {
          timer::Running<1000000> timer(state->memory()->gc_stats.total_full_concurrent_collection_time,
                                        state->memory()->gc_stats.last_full_concurrent_collection_time);
          // Allow for a young stop the world GC to occur
          // every bunch of marks. 100 is a fairly arbitrary
          // number, based mostly on the fact it didn't cause
          // big increases in young gc times because of long
          // stop the world wait times.
          while(immix_->process_mark_stack(100)) {
            state->gc_independent(gct, 0);
            state->gc_dependent(gct, 0);
          }
        }

        atomic::integer initial_stop = state->memory()->gc_stats.last_full_stop_collection_time;
        {
          timer::Running<1000000> timer(state->memory()->gc_stats.total_full_stop_collection_time,
                                      state->memory()->gc_stats.last_full_stop_collection_time);
          // Finish and pause
          while(!state->stop_the_world()) {
            state->checkpoint(gct, 0);
          }
          state->memory()->collect_mature_finish(state, data_);
          state->memory()->clear_mature_mark_in_progress();
        }
        state->memory()->gc_stats.last_full_stop_collection_time.add(initial_stop.value);
        state->memory()->print_mature_stats(state, data_);
        state->restart_world();
      }

      {
        utilities::thread::Mutex::LockGuard lg(run_lock_);
        if(data_) {
          delete data_;
          data_ = NULL;
        }
        if(exit_) break;
        state->gc_independent(gct, 0);
        paused_ = true;
        pause_cond_.signal();
        run_cond_.wait(run_lock_);
      }
      state->gc_dependent(gct, 0);
    }
    state->memory()->clear_mature_mark_in_progress();
    RUBINIUS_THREAD_STOP(thread_name, state->vm()->thread_id(), 1);
  }

}

