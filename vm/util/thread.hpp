#ifndef RBX_UTIL_THREAD_HPP
#define RBX_UTIL_THREAD_HPP

#include "vm/config.h"

#include <errno.h>
#include <signal.h>
#include <assert.h>

#include <sstream>
#include <iostream>

namespace thread {
  enum Code {
    cLocked,
    cUnlocked,
    cLockBusy,
    cNotYours,
    cReady,
    cTimedOut
  };

  /*
   * A stacklet object for locking and unlocking.
   */

#ifdef DEBUG_LOCKGUARD
  const bool cDebugLockGuard = true;
#else
  const bool cDebugLockGuard = false;
#endif

  template <class T>
  class LockGuardTemplate {
  public:
    T& lock_;
    bool locked_;

    LockGuardTemplate(T& in_lock, bool initial = false)
      : lock_(in_lock)
      , locked_(initial)
    { }

    void lock() {
      if(locked_) return;
      if(cDebugLockGuard) {
        // TODO: abstract pthread_self()
        // std::cout << "[[ " << pthread_self() << "   Locking " << lock_.describe() << " ]]\n";
      }
      lock_.lock();
      locked_ = true;
      if(cDebugLockGuard) {
        // TODO: abstract pthread_self()
        // std::cout << "[[ " << pthread_self() << "   Locked " << lock_.describe() << " ]]\n";
      }
    }

    void unlock() {
      if(!locked_) return;
      if(cDebugLockGuard) {
        // TODO: abstract pthread_self()
        // std::cout << "[[ " << pthread_self() << " Unlocking " << lock_.describe() << " ]]\n";
      }
      lock_.unlock();
      locked_ = false;
    }
  };

  template <class T>
  class StackLockGuard : public LockGuardTemplate<T> {
  public:
    StackLockGuard(T& in_lock)
      : LockGuardTemplate<T>(in_lock, false)
    {
      this->lock();
    }

    ~StackLockGuard() {
      this->unlock();
    }
  };

  template <class T>
  class StackUnlockGuard : public LockGuardTemplate<T> {
  public:
    StackUnlockGuard(T& lock_obj)
      : LockGuardTemplate<T>(lock_obj, true)
    {
      this->unlock();
    }

    ~StackUnlockGuard() {
      this->lock();
    }
  };

  // Useful for stubbing out lock usage. Either based on a compile time
  // decision about not needing a lock around something or for while debugging.
  class NullLock {
  public:
    void lock() {}
    void unlock() {}
    bool try_lock() { return cLocked; }

    std::string describe() {
      std::stringstream ss;
      ss << "NullLock ";
      ss << (void*)this;
      return ss.str();
    }
  };
}

#ifdef RBX_WINDOWS
#include "util/thread_windows.hpp"
#else
#include "util/thread_posix.hpp"
#endif

#endif
