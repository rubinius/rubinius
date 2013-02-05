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

    Object* result = cNil;
    while(!wake_) {
      {
        GCIndependent gc_guard(state, call_frame);

        cond_.wait(mutex_);
      }
      if(!state->check_async(call_frame)) {
        result = NULL;
        break;
      }
    }

    sleeping_ = false;
    state->vm()->thread->sleep(state, cFalse);
    return result;
  }

  Object* Park::park_timed(STATE, CallFrame* call_frame, struct timespec* ts) {
    utilities::thread::Mutex::LockGuard lg(mutex_);
    if(!state->check_async(call_frame)) return NULL;

    wake_ = false;
    sleeping_ = true;
    state->vm()->thread->sleep(state, cTrue);

    Object* timeout = cFalse;

    while(!wake_) {
      {
        GCIndependent gc_guard(state, call_frame);

        utilities::thread::Code status = cond_.wait_until(mutex_, ts);
        if(status == utilities::thread::cTimedOut) {
          timeout = cTrue;
          break;
        }
      }
      if(!state->check_async(call_frame)) {
        timeout = NULL;
        break;
      }
    }

    sleeping_ = false;
    state->vm()->thread->sleep(state, cFalse);

    return timeout;
  }
}
