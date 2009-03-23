#ifndef RBX_UTIL_MAILBOX_HPP
#define RBX_UTIL_MAILBOX_HPP

#include "util/thread.hpp"
#include <list>

namespace thread {
  template <typename T>
  class Mailbox {
    SpinLock lock_;
    std::list<T> messages_;

  public:
    bool empty_p() {
      SpinLock::LockGuard l(lock_);
      return messages_.size() == 0;
    }

    void add(T msg) {
      SpinLock::LockGuard l(lock_);
      messages_.push_back(msg);
    }

    T pop() {
      SpinLock::LockGuard l(lock_);
      T msg = messages_.back();
      messages_.pop_back();
      return msg;
    }
  };
}

#endif
