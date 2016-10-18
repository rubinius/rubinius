#include "defines.hpp"
#include "memory.hpp"
#include "park.hpp"
#include "state.hpp"
#include "thread_phase.hpp"
#include "vm.hpp"

#include "class/thread.hpp"

namespace rubinius {
  Object* Park::park(STATE) {
    if(state->vm()->thread_interrupted_p(state)) return NULL;

    utilities::thread::Mutex::LockGuard lg(mutex_);

    wake_ = false;
    sleeping_ = true;
    state->vm()->thread()->sleep(state, cTrue);

    Object* result = cNil;
    while(!wake_) {
      {
        UnmanagedPhase sleeping(state);

        cond_.wait(mutex_);
      }
      mutex_.unlock();
      if(state->vm()->thread_interrupted_p(state)) {
        mutex_.lock();
        result = NULL;
        break;
      }
      mutex_.lock();
    }

    sleeping_ = false;
    state->vm()->thread()->sleep(state, cFalse);
    return result;
  }

  Object* Park::park_timed(STATE, struct timespec* ts) {
    utilities::thread::Mutex::LockGuard lg(mutex_);
    if(state->vm()->thread_interrupted_p(state)) return NULL;

    wake_ = false;
    sleeping_ = true;
    state->vm()->thread()->sleep(state, cTrue);

    Object* timeout = cFalse;

    while(!wake_) {
      {
        UnmanagedPhase sleeping(state);

        utilities::thread::Code status = cond_.wait_until(mutex_, ts);
        if(status == utilities::thread::cTimedOut) {
          timeout = cTrue;
          break;
        }
      }
      mutex_.unlock();
      if(state->vm()->thread_interrupted_p(state)) {
        mutex_.lock();
        timeout = NULL;
        break;
      }
      mutex_.lock();
    }

    sleeping_ = false;
    state->vm()->thread()->sleep(state, cFalse);

    return timeout;
  }

  void Park::reset_parked() {
    mutex_.init();
    cond_.init();
  }
}
