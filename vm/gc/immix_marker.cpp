#include "config.h"
#include "configuration.hpp"
#include "immix_marker.hpp"
#include "metrics.hpp"
#include "object_memory.hpp"
#include "state.hpp"
#include "thread_phase.hpp"
#include "vm.hpp"

#include "builtin/class.hpp"
#include "builtin/thread.hpp"

#include "gc/gc.hpp"
#include "gc/immix.hpp"

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

          state->shared().thread_nexus()->blocking(state->vm());

          while(immix_->process_mark_stack(immix_->memory()->collect_young_now)) {
            if(state->shared().thread_nexus()->stop_p()) {
              state->shared().thread_nexus()->yielding(state->vm());
            }
            state->shared().thread_nexus()->blocking(state->vm());
          }
        }

        state->vm()->become_managed();

        if(thread_exit_) break;

        {
          timer::StopWatch<timer::milliseconds> timer(
              state->vm()->metrics().gc.immix_stop_ms);

          LockPhase locked(state);

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
