#ifndef RBX_LOCK_HPP
#define RBX_LOCK_HPP

#include "util/thread.hpp"

namespace rubinius {

  class Mutex;
  class SpinLock;
  class ManagedThread;

  class Lock {
  protected:
    ManagedThread* locking_thread_;
    const char* lock_file_;
    int lock_line_;

  public:
    Lock()
      : locking_thread_(0)
      , lock_file_(0)
      , lock_line_(0)
    {}

    virtual ~Lock() { }

    ManagedThread* locking_thread() {
      return locking_thread_;
    }

    const char* lock_file() const {
      return lock_file_;
    }

    int lock_line() const {
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

  class Mutex : public Lock, public utilities::thread::Mutex {
  public:
    bool mutex_p() {
      return true;
    }

    void lock(ManagedThread* th) {
      utilities::thread::Mutex::lock();
      locking_thread_ = th;
      lock_file_ = "unknown";
      lock_line_ = 0;
    }

    void lock(ManagedThread* th, const char* file, int line) {
      utilities::thread::Mutex::lock();
      locking_thread_ = th;
      lock_file_ = file;
      lock_line_ = line;
    }

    void unlock(ManagedThread* th) {
      locking_thread_ = 0;
      utilities::thread::Mutex::unlock();
      lock_file_ = "";
      lock_line_ = 0;
    }
  };

  class SpinLock : public Lock, public utilities::thread::SpinLock {
  public:
    bool mutex_p() {
      return false;
    }

    void lock(ManagedThread* th) {
      utilities::thread::SpinLock::lock();
      locking_thread_ = th;
      lock_file_ = "unknown";
      lock_line_ = 0;
    }

    void lock(ManagedThread* th, const char* file, int line) {
      utilities::thread::SpinLock::lock();
      locking_thread_ = th;
      lock_file_ = file;
      lock_line_ = line;
    }

    void unlock(ManagedThread* th) {
      locking_thread_ = 0;
      utilities::thread::SpinLock::unlock();
      lock_file_ = "";
      lock_line_ = 0;
    }
  };

  class ScopeLock {
    ManagedThread* thread_;
    Lock* lock_;

  public:
    ScopeLock(ManagedThread* th, Lock* lock, const char* file="unknown", int line=0)
      : thread_(th)
      , lock_(lock)
    {
      if(Mutex* mutex = lock_->as_mutex()) {
        mutex->lock(thread_, file, line);
      } else if(SpinLock* sl = lock_->as_spinlock()) {
        sl->lock(thread_, file, line);
      } else {
        std::cerr << "Syncronize used with confusing lock type.\n";
      }
    }

    ScopeLock(ManagedThread* th, Lock& lock, const char* file="unknown", int line=0)
      : thread_(th)
      , lock_(&lock)
    {
      if(Mutex* mutex = lock_->as_mutex()) {
        mutex->lock(thread_, file, line);
      } else if(SpinLock* sl = lock_->as_spinlock()) {
        sl->lock(thread_, file, line);
      } else {
        std::cerr << "Syncronize used with confusing lock type.\n";
      }
    }

    ~ScopeLock() {
      if(Mutex* mutex = lock_->as_mutex()) {
        mutex->unlock(thread_);
      } else if(SpinLock* sl = lock_->as_spinlock()) {
        sl->unlock(thread_);
      } else {
        std::cerr << "Syncronize used with confusing lock type.\n";
      }
    }
  };

#define SCOPE_LOCK(state, lock) rubinius::ScopeLock __scope_lock__(state, lock, __FILE__, __LINE__)

  class Lockable {
    Mutex mutex_;

  public:
    Mutex& mutex() {
      return mutex_;
    }

    void lock_init(ManagedThread* th) {
      mutex_.init(th);
    }

    void lock(ManagedThread* th) {
      mutex_.lock(th);
    }

    void lock(ManagedThread* th, const char* file, int line) {
      mutex_.lock(th, file, line);
    }

    void unlock(ManagedThread* th) {
      mutex_.unlock(th);
    }
  };

  class LockableScopedLock {
    ManagedThread* thread_;
    Lockable* lock_;
    bool locked_;
    bool recursive_;

  public:
    LockableScopedLock(ManagedThread* th, Lockable* lock,
                       const char* file="unknown", int line=0)
      : thread_(th)
      , lock_(lock)
      , locked_(false)
      , recursive_(false)
    {
      if(th && lock_->mutex().locking_thread() == th) {
        recursive_ = true;
        // Don't relock, already got it locked.
      } else {
        lock_->lock(thread_, file, line);
        locked_ = true;
      }
    }

    LockableScopedLock(State* state, Lockable* lock,
                       const char* file="unknown", int line=0);

    void unlock() {
      if(recursive_) return;

      if(locked_) {
        locked_ = false;
        lock_->unlock(thread_);
      }
    }

    void relock() {
      if(recursive_) return;

      if(!locked_) {
        lock_->lock(thread_);
        locked_ = true;
      }
    }

    ~LockableScopedLock() {
      if(locked_) lock_->unlock(thread_);
    }
  };


#define SYNC(__state) LockableScopedLock __lsl_guard__(__state, this, __FILE__, __LINE__)
#define SYNC_TL LockableScopedLock __lsl_guard__(ManagedThread::current(), this, __FILE__, __LINE__)

#define UNSYNC __lsl_guard__.unlock()
#define RESYNC __lsl_guard__.relock()
}

#endif
