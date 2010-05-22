#ifndef RBX_GLOBAL_LOCK_HPP
#define RBX_GLOBAL_LOCK_HPP

#include "util/thread.hpp"

namespace rubinius {
  class VM;
  class CallFrame;
  class NativeMethodEnvironment;

  class GlobalLock : public thread::Mutex {
  public:

    static bool debug_locking;

    void yield(VM* vm, CallFrame* call_frame);

    class LockGuard : public thread::LockGuardTemplate<GlobalLock> {
    public:
      LockGuard(GlobalLock& in_lock)
        : thread::LockGuardTemplate<GlobalLock>(in_lock, false)
      {
        if(debug_locking) std::cout << "[  Locking GIL ] " << pthread_self() << "\n";
        this->lock();
        if(debug_locking) std::cout << "[   Locked GIL ] " << pthread_self() << "\n";
      }

      ~LockGuard() {
        this->unlock();
        if(debug_locking) std::cout << "[ Unlocked GIL ] " << pthread_self() << "\n";
      }
    };

    class UnlockGuard : public thread::LockGuardTemplate<GlobalLock> {
    public:
      UnlockGuard(VM* state, CallFrame* call_frame);
      UnlockGuard(NativeMethodEnvironment* nme);

      ~UnlockGuard() {
        if(debug_locking) std::cout << "[  Locking GIL ] " << pthread_self() << "\n";
        this->lock();
        if(debug_locking) std::cout << "[   Locked GIL ] " << pthread_self() << "\n";
      }
    };
  };
}

#endif
