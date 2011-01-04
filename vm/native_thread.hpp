#ifndef RBX_NATIVE_THREAD_HPP
#define RBX_NATIVE_THREAD_HPP

#include "util/thread.hpp"

namespace rubinius {
  class VM;

  class NativeThread : public thread::Thread {
    VM* vm_;

  public:
    NativeThread(VM*, size_t stack_size = 0, thread_t tid = 0);
    void perform();

    VM* vm() {
      return vm_;
    }

  };

  class Waiter {
    bool used_;

  public:
    Waiter()
      : used_(false)
    {}

    virtual ~Waiter() { }
    virtual void wakeup() = 0;

    void run() {
      used_ = true;
      wakeup();
    }

    bool used() {
      return used_;
    }
  };

  class WaitingOnCondition : public Waiter {
    thread::Condition& cond_;
    thread::Mutex& mutex_;

  public:
    WaitingOnCondition(thread::Condition& cond, thread::Mutex& mutex)
      : cond_(cond)
      , mutex_(mutex)
    {}

    void wakeup() {
      thread::Mutex::LockGuard lg(mutex_);
      cond_.signal();
    }
  };

  class WaitingForSignal : public Waiter {
    NativeThread::thread_t target_;

  public:
    WaitingForSignal()
      : target_(NativeThread::self())
    {}

    void wakeup() {
      NativeThread::signal(target_, NativeThread::cWakeupSignal);
    }
  };
}

#endif
