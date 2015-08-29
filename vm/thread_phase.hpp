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
      state->shared().thread_nexus()->lock_or_wait(state->vm());
    }

    ~LockPhase() {
      state_->shared().thread_nexus()->unlock();
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
}

#endif
