#ifndef RBX_PARK_HPP
#define RBX_PARK_HPP

#include "util/thread.hpp"

namespace rubinius {
  struct CallFrame;

  class Park {
    thread::Condition cond_;
    thread::Mutex mutex_;
    bool sleeping_;
    bool wake_;

  public:
    Park()
      : sleeping_(false)
      , wake_(false)
    {}

    void unpark() {
      thread::Mutex::LockGuard lg(mutex_);
      if(!sleeping_) return;

      wake_ = true;
      cond_.signal();
    }

    bool parked_p() {
      thread::Mutex::LockGuard lg(mutex_);
      return sleeping_;
    }

    void park(STATE, CallFrame* call_frame);
    bool park_timed(STATE, CallFrame* call_frame, struct timespec* ts);
  };
}

#endif
