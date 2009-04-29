#ifndef RBX_NATIVE_THREAD_HPP
#define RBX_NATIVE_THREAD_HPP

#include "global_lock.hpp"
#include "vm.hpp"

#include "builtin/array.hpp"

namespace rubinius {
  typedef thread::Mutex GlobalLock;

  class NativeThread : public thread::Thread {
    VM* vm_;

  public:
    NativeThread(VM*, size_t stack_size = 0, pthread_t tid = 0);
    void perform();

    VM* vm() {
      return vm_;
    }

    static void block_all_signals() {
      sigset_t set;
      sigfillset(&set);

      pthread_sigmask(SIG_SETMASK, &set, NULL);
    }

  };

  class Waiter {
  public:
    virtual ~Waiter() { }
    virtual void wakeup() = 0;
  };

  class WaitingOnCondition : public Waiter {
    thread::Condition& cond_;

  public:
    WaitingOnCondition(thread::Condition& cond)
      : cond_(cond)
    {}

    void wakeup() {
      cond_.signal();
    }
  };

  class WaitingForSignal : public Waiter {
    pthread_t target_;

  public:
    WaitingForSignal()
      : target_(pthread_self())
    {}

    void wakeup() {
      pthread_kill(target_, 7);
    }
  };
}

#endif
