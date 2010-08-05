#ifndef RBX_LOCK_HPP
#define RBX_LOCK_HPP

#include "util/thread.hpp"

namespace rubinius {

  class Mutex;
  class SpinLock;

  class Lock {
  protected:
    VM* locking_thread_;
    const char* lock_file_;
    int lock_line_;

  public:
    VM* locking_thread() {
      return locking_thread_;
    }

    const char* lock_file() {
      return lock_file_;
    }

    int lock_line() {
      return lock_line_;
    }

    virtual bool mutex_p() = 0;

    Mutex* as_mutex() {
      if(mutex_p()) return reinterpret_cast<Mutex*>(this);
      return 0;
    }

    SpinLock* as_spinlock() {
      if(!mutex_p()) return reinterpret_cast<SpinLock*>(this);
      return 0;
    }
  };

  class Mutex : public Lock, public thread::Mutex {
  public:
    bool mutex_p() {
      return true;
    }

    void lock(STATE) {
      thread::Mutex::lock();
      locking_thread_ = state;
      lock_file_ = "unknown";
      lock_line_ = 0;
    }

    void lock(STATE, const char* file, int line) {
      thread::Mutex::lock();
      locking_thread_ = state;
      lock_file_ = file;
      lock_line_ = line;
    }

    void unlock(STATE) {
      thread::Mutex::unlock();
      locking_thread_ = 0;
    }
  };

  class SpinLock : public Lock, public thread::SpinLock {
  public:
    bool mutex_p() {
      return false;
    }

    void lock(STATE) {
      thread::SpinLock::lock();
      locking_thread_ = state;
      lock_file_ = "unknown";
      lock_line_ = 0;
    }

    void lock(STATE, const char* file, int line) {
      thread::SpinLock::lock();
      locking_thread_ = state;
      lock_file_ = file;
      lock_line_ = line;
    }

    void unlock(STATE) {
      thread::SpinLock::lock();
      locking_thread_ = 0;
    }
  };

  class ScopeLock {
    VM* vm_;
    Lock* lock_;

  public:
    ScopeLock(STATE, Lock* lock, const char* file="unknown", int line=0)
      : vm_(state)
      , lock_(lock)
    {
      if(Mutex* mutex = lock_->as_mutex()) {
        mutex->lock(vm_, file, line);
      } else if(SpinLock* sl = lock_->as_spinlock()) {
        sl->lock(vm_, file, line);
      } else {
        std::cerr << "Syncronize used with confusing lock type.\n";
      }
    }

    ScopeLock(STATE, Lock& lock, const char* file="unknown", int line=0)
      : vm_(state)
      , lock_(&lock)
    {
      if(Mutex* mutex = lock_->as_mutex()) {
        mutex->lock(vm_, file, line);
      } else if(SpinLock* sl = lock_->as_spinlock()) {
        sl->lock(vm_, file, line);
      } else {
        std::cerr << "Syncronize used with confusing lock type.\n";
      }
    }

    ~ScopeLock() {
      if(Mutex* mutex = lock_->as_mutex()) {
        mutex->unlock(vm_);
      } else if(SpinLock* sl = lock_->as_spinlock()) {
        sl->unlock(vm_);
      } else {
        std::cerr << "Syncronize used with confusing lock type.\n";
      }
    }
  };

  class Lockable {
    Mutex mutex_;

  public:
    Mutex& mutex() {
      return mutex_;
    }

    void lock(STATE) {
      mutex_.lock(state);
    }

    void lock(STATE, const char* file, int line) {
      mutex_.lock(state, file, line);
    }

    void unlock(STATE) {
      mutex_.unlock(state);
    }
  };

  class LockableScopedLock {
    VM* vm_;
    Lockable* lock_;
    bool locked_;
    bool recursive_;

  public:
    LockableScopedLock(STATE, Lockable* lock, const char* file="unknown", int line=0)
      : vm_(state)
      , lock_(lock)
      , locked_(false)
      , recursive_(false)
    {
      if(lock_->mutex().locking_thread() == state) {
        recursive_ = true;
        // Don't relock, already got it locked.
      } else {
        lock_->lock(vm_, file, line);
        locked_ = true;
      }
    }

    void unlock() {
      if(recursive_) return;

      if(locked_) {
        lock_->unlock(vm_);
        locked_ = false;
      }
    }

    void relock() {
      if(recursive_) return;

      if(!locked_) {
        lock_->lock(vm_);
        locked_ = true;
      }
    }

    ~LockableScopedLock() {
      if(locked_) lock_->unlock(vm_);
    }
  };

#define SYNC(vm) LockableScopedLock __lsl_guard__(vm, this, __FILE__, __LINE__)
#define SYNC_TL LockableScopedLock __lsl_guard__(VM::current(), this, __FILE__, __LINE__)

#define UNSYNC __lsl_guard__.unlock()
#define RESYNC __lsl_guard__.relock()
}

#endif
