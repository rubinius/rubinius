#include "prelude.hpp"
#include "park.hpp"
#include "builtin/thread.hpp"

namespace rubinius {
  Object* Park::park(STATE, CallFrame* call_frame) {
    utilities::thread::Mutex::LockGuard lg(mutex_);
    if(!state->check_async(call_frame)) return NULL;

    wake_ = false;
    sleeping_ = true;
    state->vm()->thread->sleep(state, cTrue);

    while(!wake_) {
      GCIndependent gc_guard(state, call_frame);

      cond_.wait(mutex_);
    }

    sleeping_ = false;
    state->vm()->thread->sleep(state, cFalse);
    return cNil;
  }

  Object* Park::park_timed(STATE, CallFrame* call_frame, struct timespec* ts) {
    utilities::thread::Mutex::LockGuard lg(mutex_);
    if(!state->check_async(call_frame)) return NULL;

    wake_ = false;
    sleeping_ = true;
    state->vm()->thread->sleep(state, cTrue);

    Object* timeout = cFalse;

    while(!wake_) {
      GCIndependent gc_guard(state, call_frame);

      if(cond_.wait_until(mutex_, ts) == utilities::thread::cTimedOut) {
        timeout = cTrue;
        break;
      }
    }

    sleeping_ = false;
    state->vm()->thread->sleep(state, cFalse);

    return timeout;
  }
}
