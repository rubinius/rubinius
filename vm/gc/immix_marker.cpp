#include "config.h"
#include "vm.hpp"
#include "state.hpp"
#include "immix_marker.hpp"

#include "builtin/class.hpp"
#include "builtin/thread.hpp"
#include "configuration.hpp"
#include "gc/gc.hpp"
#include "gc/immix.hpp"
#include "ontology.hpp"
#include "metrics.hpp"
#include "thread_phase.hpp"

#include "dtrace/dtrace.h"
#include "instruments/timing.hpp"

namespace rubinius {

  ImmixMarker::ImmixMarker(STATE, ImmixGC* immix)
    : InternalThread(state, "rbx.immix")
    , immix_(immix)
    , data_(NULL)
  {
    state->memory()->set_immix_marker(this);

    initialize(state);
    start_thread(state);
  }

  ImmixMarker::~ImmixMarker() {
    cleanup();
  }

  void ImmixMarker::initialize(STATE) {
    InternalThread::initialize(state);

    Thread::create(state, vm());

    run_lock_.init();
    run_cond_.init();
  }

  void ImmixMarker::wakeup(STATE) {
    utilities::thread::Mutex::LockGuard lg(run_lock_);

    InternalThread::wakeup(state);

    run_cond_.signal();
  }

  void ImmixMarker::after_fork_child(STATE) {
    cleanup();

    InternalThread::after_fork_child(state);
  }

  void ImmixMarker::cleanup() {
    if(data_) {
      delete data_;
      data_ = NULL;
    }
  }

  void ImmixMarker::stop(STATE) {
    InternalThread::stop(state);
  }

  void ImmixMarker::concurrent_mark(GCData* data) {
    utilities::thread::Mutex::LockGuard lg(run_lock_);
    data_ = data;
    run_cond_.signal();
  }

  void ImmixMarker::run(STATE) {
    state->vm()->become_managed();

    while(!thread_exit_) {
      if(data_) {
        {
          timer::StopWatch<timer::milliseconds> timer(
              state->vm()->metrics().gc.immix_concurrent_ms);

          // Allow for a young stop the world GC to occur
          // every bunch of marks. 100 is a fairly arbitrary
          // number, based mostly on the fact it didn't cause
          // big increases in young gc times because of long
          // stop the world wait times.
          while(immix_->process_mark_stack(100)) {
            if(state->shared().thread_nexus()->stop_p()) {
              state->shared().thread_nexus()->yielding(state->vm());
            }
          }
        }

        if(thread_exit_) break;

        {
          LockPhase locked(state);

          timer::StopWatch<timer::milliseconds> timer(
              state->vm()->metrics().gc.immix_stop_ms);

          state->memory()->clear_mature_mark_in_progress();
          state->memory()->collect_mature_finish(state, data_);
        }
      }

      {
        utilities::thread::Mutex::LockGuard guard(run_lock_);

        cleanup();
        if(thread_exit_) break;

        {
          UnmanagedPhase unmanaged(state);
          run_cond_.wait(run_lock_);
        }
      }
    }

    state->memory()->clear_mature_mark_in_progress();
  }
}
