#ifndef RBX_THREAD_PHASE_HPP
#define RBX_THREAD_PHASE_HPP

#include "defines.hpp"
#include "vm.hpp"
#include "state.hpp"
#include "thread_nexus.hpp"
#include "memory.hpp"

namespace rubinius {
  /**
   * Instantiation of an instance of this class causes Ruby execution on all
   * threads to be suspended. Upon destruction of the instance, Ruby execution
   * is resumed.
   */
  class StopPhase {
    State* state_;

  public:
    StopPhase(STATE)
      : state_(state)
    {
      // TODO: GC make thread_nexus aware of this
      while(true) {
        if(!state->shared().memory()->collector()->collect_requested_p()) {
          state->vm()->thread_nexus()->stop(state, state->vm());
          break;
        }
      }
    }

    ~StopPhase() {
      state_->vm()->thread_nexus()->unset_stop();
      state_->vm()->thread_nexus()->unlock(state_, state_->vm());
    }
  };

  class ManagedPhase {
    State* state_;

  public:
    ManagedPhase(STATE)
      : state_(state)
    {
      state_->vm()->managed_phase(state_);
    }

    ~ManagedPhase() {
      state_->vm()->unmanaged_phase(state_);
    }

  };

  class UnmanagedPhase {
    State* state_;

  public:
    UnmanagedPhase(STATE)
      : state_(state)
    {
      state_->vm()->unmanaged_phase(state_);
    }

    ~UnmanagedPhase() {
      state_->vm()->managed_phase(state_);
    }
  };

  template <typename T>
  class LockWaiting {
    T& lock_;

  public:
    LockWaiting(STATE, T& in_lock)
      : lock_(in_lock)
    {
      state->vm()->thread_nexus()->waiting_phase(state, state->vm());

      while(true) {
        lock_.lock();

        if(state->vm()->thread_nexus()->try_managed_phase(state, state->vm())) {
          return;
        } else {
          lock_.unlock();
        }
      }
    }

    ~LockWaiting() {
      lock_.unlock();
    }
  };

  typedef LockWaiting<utilities::thread::Mutex> MutexLockWaiting;
  typedef LockWaiting<utilities::thread::SpinLock> SpinLockWaiting;
}

#endif
