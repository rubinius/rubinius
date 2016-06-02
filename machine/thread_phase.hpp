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

  public:
    LockPhase(STATE)
      : state_(state)
    {
      state->vm()->thread_nexus()->lock(state->vm());
    }

    ~LockPhase() {
      state_->vm()->thread_nexus()->unlock();
    }
  };

  class BlockPhase {
    State* state_;

  public:
    BlockPhase(STATE)
      : state_(state)
    {
      state->vm()->blocking_phase();
    }

    ~BlockPhase() {
      state_->vm()->managed_phase();
    }
  };

  class ManagedPhase {
    State* state_;

  public:
    ManagedPhase(STATE)
      : state_(state)
    {
      state_->vm()->managed_phase();
    }

    ~ManagedPhase() {
      state_->vm()->unmanaged_phase();
    }

  };

  class UnmanagedPhase {
    State* state_;

  public:
    UnmanagedPhase(STATE)
      : state_(state)
    {
      state_->vm()->unmanaged_phase();
    }

    ~UnmanagedPhase() {
      state_->vm()->managed_phase();
    }
  };

  template <class T>
  class LockWaiting : public utilities::thread::LockGuardTemplate<T> {
    State* state_;
    ThreadNexus::Phase phase_;

  public:
    LockWaiting(STATE, T& in_lock)
      : utilities::thread::LockGuardTemplate<T>(in_lock, false)
      , state_(state)
    {
      phase_ = state_->vm()->thread_phase();
      state_->vm()->thread_nexus()->waiting_phase(state_->vm());

      this->lock();

      state_->vm()->thread_nexus()->restore_phase(state_->vm(), phase_);
    }

    ~LockWaiting() {
      this->unlock();
    }
  };

  typedef LockWaiting<utilities::thread::Mutex> MutexLockWaiting;
  typedef LockWaiting<utilities::thread::SpinLock> SpinLockWaiting;
}

#endif
