#ifndef RBX_GLOBAL_LOCK_HPP
#define RBX_GLOBAL_LOCK_HPP

#include "util/thread.hpp"

#include <sys/time.h>
#include <stdint.h>

namespace rubinius {
  class VM;
  class CallFrame;
  class NativeMethodEnvironment;
  class SharedState;

  class GlobalLock {
    uint32_t locked_;
    uint32_t serial_;
    uint32_t request_drop_;

    thread::Mutex mutex_;
    thread::Condition condition_;

    thread::Mutex handshake_mutex_;
    thread::Condition handshake_condition_;

    struct timeval timeout_;

  public:
    static bool debug_locking;

    bool should_yield() {
      return request_drop_ == 1;
    }

    void checkpoint(VM* state, CallFrame* call_frame) {
      if(should_yield()) yield(state, call_frame);
    }

    GlobalLock();

    void init();

    void drop();
    void take();
    void yield(VM* vm, CallFrame* call_frame);

    class LockGuard {
      GlobalLock& lock_;

    public:
      LockGuard(GlobalLock& in_lock);
      ~LockGuard();
    };

    class UnlockGuard {
      VM* vm_;
      GlobalLock& lock_;

    public:
      UnlockGuard(VM* state, CallFrame* call_frame);
      UnlockGuard(NativeMethodEnvironment* nme);
      ~UnlockGuard();
    };
  };
}

#endif
