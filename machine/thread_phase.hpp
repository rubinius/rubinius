#ifndef RBX_THREAD_PHASE_HPP
#define RBX_THREAD_PHASE_HPP

#include "defines.hpp"
#include "thread_state.hpp"
#include "memory.hpp"
#include "spinlock.hpp"

#include "memory/collector.hpp"

#include <mutex>

namespace rubinius {
  /**
   * Instantiation of an instance of this class causes Ruby execution on all
   * threads to be suspended. Upon destruction of the instance, Ruby execution
   * is resumed.
   */
  class StopPhase {
    ThreadState* state_;

  public:
    StopPhase(STATE)
      : state_(state)
    {
      while(true) {
        if(state->collector()->collect_requested_p()) {
          state->yield();
        } else {
          state->stop();
          break;
        }
      }
    }

    ~StopPhase() {
      state_->unset_stop();
      state_->unlock();
    }
  };

  class ManagedPhase {
    ThreadState* state_;

  public:
    ManagedPhase(STATE)
      : state_(state)
    {
      state_->managed_phase();
    }

    ~ManagedPhase() {
      state_->unmanaged_phase();
    }

  };

  class UnmanagedPhase {
    ThreadState* state_;

  public:
    UnmanagedPhase(STATE)
      : state_(state)
    {
      state_->unmanaged_phase();
    }

    ~UnmanagedPhase() {
      state_->managed_phase();
    }
  };

  template <typename T>
  class LockWaiting {
    T& lock_;

  public:
    LockWaiting(STATE, T& in_lock)
      : lock_(in_lock)
    {
      state->waiting_phase();

      while(true) {
        lock_.lock();

        if(state->try_managed_phase()) {
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

  typedef LockWaiting<std::mutex> MutexLockWaiting;
  typedef LockWaiting<locks::spinlock_mutex> SpinLockWaiting;
}

#endif
