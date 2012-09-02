#ifndef RBX_PARK_HPP
#define RBX_PARK_HPP

#include "util/thread.hpp"

namespace rubinius {
  struct CallFrame;
  class Object;

  class Park {
    utilities::thread::Condition cond_;
    utilities::thread::Mutex mutex_;
    bool sleeping_;
    bool wake_;

  public:
    Park()
      : sleeping_(false)
      , wake_(false)
    {}

    void unpark() {
      utilities::thread::Mutex::LockGuard lg(mutex_);
      if(!sleeping_) return;

      wake_ = true;
      cond_.signal();
    }

    bool parked_p() {
      utilities::thread::Mutex::LockGuard lg(mutex_);
      return sleeping_;
    }

    Object* park(STATE, CallFrame* call_frame);
    Object* park_timed(STATE, CallFrame* call_frame, struct timespec* ts);
  };
}

#endif
