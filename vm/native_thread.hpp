#ifndef RBX_NATIVE_THREAD_HPP
#define RBX_NATIVE_THREAD_HPP

#include "global_lock.hpp"
#include "vm.hpp"

#include "builtin/array.hpp"

namespace rubinius {
  typedef thread::Mutex GlobalLock;

  class NativeThread : public thread::Thread {
    VM* vm_;
    GlobalLock& lock_;

    TypedRoot<Object*> block_;
    TypedRoot<Array*> args_;

  public:
    NativeThread(VM*);
    void perform();

    void set_startup(Object* block, Array* args) {
      block_.set(block);
      args_.set(args);
    }

    static void block_all_signals() {
      sigset_t set;
      sigfillset(&set);

      pthread_sigmask(SIG_BLOCK, &set, NULL);
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
}

#endif
