#include "prelude.hpp"
#include "park.hpp"
#include "builtin/thread.hpp"

namespace rubinius {
  void Park::park(STATE, CallFrame* call_frame) {
    thread::Mutex::LockGuard lg(mutex_);

    wake_ = false;
    sleeping_ = true;
    state->vm()->thread->sleep(state, cTrue);

    while(!wake_) {
      GCIndependent gc_guard(state, call_frame);

      cond_.wait(mutex_);
    }

    sleeping_ = false;
    state->vm()->thread->sleep(state, cFalse);
  }

  bool Park::park_timed(STATE, CallFrame* call_frame, struct timespec* ts) {
    thread::Mutex::LockGuard lg(mutex_);

    wake_ = false;
    sleeping_ = true;
    state->vm()->thread->sleep(state, cTrue);

    bool timeout = false;

    while(!wake_) {
      GCIndependent gc_guard(state, call_frame);

      if(cond_.wait_until(mutex_, ts) == thread::cTimedOut) {
        timeout = true;
        break;
      }
    }

    sleeping_ = false;
    state->vm()->thread->sleep(state, cFalse);

    return timeout;
  }
}
