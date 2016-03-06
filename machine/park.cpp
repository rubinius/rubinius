#include "defines.hpp"
#include "memory.hpp"
#include "park.hpp"
#include "state.hpp"
#include "thread_phase.hpp"
#include "vm.hpp"

#include "builtin/thread.hpp"

namespace rubinius {
  Object* Park::park(STATE) {
    if(!state->check_async(state)) return NULL;

    utilities::thread::Mutex::LockGuard lg(mutex_);

    wake_ = false;
    sleeping_ = true;
    state->vm()->thread->sleep(state, cTrue);

    Object* result = cNil;
    while(!wake_) {
      {
        SleepPhase sleeping(state);

        cond_.wait(mutex_);
      }
      mutex_.unlock();
      if(!state->check_async(state)) {
        mutex_.lock();
        result = NULL;
        break;
      }
      mutex_.lock();
    }

    sleeping_ = false;
    state->vm()->thread->sleep(state, cFalse);
    return result;
  }

  Object* Park::park_timed(STATE, struct timespec* ts) {
    utilities::thread::Mutex::LockGuard lg(mutex_);
    if(!state->check_async(state)) return NULL;

    wake_ = false;
    sleeping_ = true;
    state->vm()->thread->sleep(state, cTrue);

    Object* timeout = cFalse;

    while(!wake_) {
      {
        SleepPhase sleeping(state);

        utilities::thread::Code status = cond_.wait_until(mutex_, ts);
        if(status == utilities::thread::cTimedOut) {
          timeout = cTrue;
          break;
        }
      }
      mutex_.unlock();
      if(!state->check_async(state)) {
        mutex_.lock();
        timeout = NULL;
        break;
      }
      mutex_.lock();
    }

    sleeping_ = false;
    state->vm()->thread->sleep(state, cFalse);

    return timeout;
  }

  void Park::reset_parked() {
    mutex_.init();
    cond_.init();
  }
}
