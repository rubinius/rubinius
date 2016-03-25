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
      state->shared().thread_nexus()->lock(state->vm());
    }

    ~LockPhase() {
      state_->shared().thread_nexus()->unlock();
    }
  };

  class SleepPhase {
    State* state_;

  public:
    SleepPhase(STATE)
      : state_(state)
    {
      state->vm()->set_thread_phase(ThreadNexus::cSleeping);
    }

    ~SleepPhase() {
      state_->shared().thread_nexus()->sleep_lock(state_->vm());
    }
  };

  class ManagedPhase {
    State* state_;

  public:
    ManagedPhase(STATE)
      : state_(state)
    {
      state_->vm()->become_managed();
    }

    ~ManagedPhase() {
      state_->vm()->become_unmanaged();
    }

  };

  class UnmanagedPhase {
    State* state_;

  public:
    UnmanagedPhase(STATE)
      : state_(state)
    {
      state_->vm()->become_unmanaged();
    }

    ~UnmanagedPhase() {
      state_->vm()->become_managed();
    }
  };

  template <class T>
  class LockUnmanaged : public utilities::thread::LockGuardTemplate<T> {
    State* state_;

  public:
    LockUnmanaged(STATE, T& in_lock)
      : utilities::thread::LockGuardTemplate<T>(in_lock, false)
      , state_(state)
    {
      state_->vm()->become_unmanaged();
      this->lock();
      state_->vm()->become_managed();
    }

    ~LockUnmanaged() {
      this->unlock();
    }
  };

  typedef LockUnmanaged<utilities::thread::Mutex> MutexLockUnmanaged;

  template <class T>
  class LockWaiting : public utilities::thread::LockGuardTemplate<T> {
    State* state_;
    ThreadNexus::Phase phase_;

  public:
    LockWaiting(STATE, T& in_lock)
      : utilities::thread::LockGuardTemplate<T>(in_lock, false)
      , state_(state)
    {
      phase_ = state->vm()->thread_phase();
      state->vm()->set_thread_phase(ThreadNexus::cWaiting);

      this->lock();
    }

    ~LockWaiting() {
      this->unlock();
      state_->vm()->restore_thread_phase(phase_);
    }
  };

  typedef LockWaiting<utilities::thread::SpinLock> SpinLockWaiting;
}

#endif
