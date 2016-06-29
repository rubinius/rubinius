#ifndef RBX_THREAD_PHASE_HPP
#define RBX_THREAD_PHASE_HPP

#include "defines.hpp"
#include "vm.hpp"
#include "state.hpp"
#include "thread_nexus.hpp"

namespace rubinius {
  /**
   * Instantiation of an instance of this class causes Ruby execution on all
   * threads to be suspended. Upon destruction of the instance, Ruby execution
   * is resumed.
   */
  class LockPhase {
    State* state_;
    ThreadNexus::LockStatus status_;

  public:
    LockPhase(STATE)
      : state_(state)
    {
      status_ = state->vm()->thread_nexus()->lock(state, state->vm());
    }

    ~LockPhase() {
      if(status_ == ThreadNexus::eLocked) {
        state_->vm()->thread_nexus()->unlock();
      }
    }
  };

  class BlockPhase {
    State* state_;

  public:
    BlockPhase(STATE)
      : state_(state)
    {
      state->vm()->blocking_phase(state_);
    }

    ~BlockPhase() {
      state_->vm()->managed_phase(state_);
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

      lock_.lock();

      state->vm()->thread_nexus()->managed_phase(state, state->vm());
    }

    ~LockWaiting() {
      lock_.unlock();
    }
  };

  typedef LockWaiting<utilities::thread::Mutex> MutexLockWaiting;
  typedef LockWaiting<utilities::thread::SpinLock> SpinLockWaiting;
}

#endif
