#ifndef RBX_UTIL_THREAD_HPP
#define RBX_UTIL_THREAD_HPP

// #define DEBUG_LOCKGUARD

#define USE_PTHREADS

#ifdef USE_PTHREADS
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>

#include <sstream>
#include <iostream>

#define pthread_check(expr) if((expr) != 0) { assert(0 && "failed: " #expr); }

namespace thread {
  enum Code {
    cLocked,
    cUnlocked,
    cLockBusy,
    cNotYours,
    cReady,
    cTimedOut
  };

#if 0
  static void block_all_signals() {
    sigset_t set;
    sigfillset(&set);

    pthread_sigmask(SIG_BLOCK, &set, NULL);
  }

  static void accept_signal(int sig) {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, sig);

    pthread_sigmask(SIG_UNBLOCK, &set, NULL);
  }

  static void ignore_signal(int sig) {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, sig);

    pthread_sigmask(SIG_BLOCK, &set, NULL);
  }

  static void wait_on_signal(int sig) {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, sig);

    int bunk;
    sigwait(&set, &bunk);
  }
#endif
  template <typename T = void*>
  class ThreadData {
    pthread_key_t native_;

  public:
    ThreadData() {
      pthread_check(pthread_key_create(&native_, NULL));
    }

    ~ThreadData() {
      pthread_check(pthread_key_delete(native_));
    }

    T get() {
      return reinterpret_cast<T>(pthread_getspecific(native_));
    }

    void set(T val) {
      pthread_check(pthread_setspecific(native_, reinterpret_cast<void*>(val)));
    }
  };

  class Thread {
    pthread_t native_;
    bool delete_on_exit_;
    size_t stack_size_;

    static void* trampoline(void* arg) {
      Thread* self = reinterpret_cast<Thread*>(arg);
      self->perform();
      if(self->delete_on_exit()) delete self;
      return NULL;
    }

  public:
    Thread(size_t stack_size = 0, pthread_t tid = 0)
      : native_(tid)
      , stack_size_(stack_size)
    {
      delete_on_exit_ = (tid != 0);
    }

    virtual ~Thread() { }

    pthread_t* native() {
      return &native_;
    }

    int run() {
      pthread_attr_t attrs;
      pthread_attr_init(&attrs);
      if(stack_size_) {
        pthread_attr_setstacksize(&attrs, stack_size_);
      }

      return pthread_create(&native_, &attrs, trampoline, (void*)this);
    }

    virtual void perform() { }

    void detach() {
      pthread_check(pthread_detach(native_));
    }

    bool equal(Thread& other) {
      if(pthread_equal(native_, *other.native())) {
        return true;
      }

      return false;
    }

    void join() {
      void* bunk;
      int err = pthread_join(native_, &bunk);
      if(err != 0) {
        if(err == EDEADLK) {
          std::cout << "Thread deadlock!\n";
          assert(0);
        }

        // Ignore the other errors, since they mean there is no thread
        // so we can consider us already joined to it.
      }
    }

    void cancel() {
      pthread_check(pthread_cancel(native_));
    }

    void kill(int sig) {
      pthread_check(pthread_kill(native_, sig));
    }

    int priority() {
      int _policy;
      struct sched_param params;

      pthread_check(pthread_getschedparam(native_, &_policy, &params));

      return params.sched_priority;
    }

    bool set_priority(int priority) {
      int _policy;
      struct sched_param params;

      pthread_check(pthread_getschedparam(native_, &_policy, &params));
      int max = sched_get_priority_max(_policy);
      int min = sched_get_priority_min(_policy);

      if(min > priority) {
        priority = min;
      }
      else if(max < priority) {
        priority = max;
      }

      params.sched_priority = priority;

      int err = pthread_setschedparam(native_, _policy, &params);
      if(err == ENOTSUP) return false;

      return true;
    }

    bool delete_on_exit() {
      return delete_on_exit_;
    }

    void set_delete_on_exit() {
      delete_on_exit_ = true;
    }
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
        std::cout << "[[ " << pthread_self() << "   Locking " << lock_.describe() << " ]]\n";
      }
      lock_.lock();
      locked_ = true;
      if(cDebugLockGuard) {
        std::cout << "[[ " << pthread_self() << "   Locked " << lock_.describe() << " ]]\n";
      }
    }

    void unlock() {
      if(!locked_) return;
      if(cDebugLockGuard) {
        std::cout << "[[ " << pthread_self() << " Unlocking " << lock_.describe() << " ]]\n";
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

  class Mutex {
  public: // Types
    typedef StackLockGuard<Mutex> LockGuard;
    typedef StackUnlockGuard<Mutex> UnlockGuard;

  private:
    pthread_mutex_t native_;

  public:
    void init() {
      pthread_mutexattr_t attr;
      pthread_mutexattr_init(&attr);
      pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
      pthread_check(pthread_mutex_init(&native_, &attr));
    }

    Mutex() {
      init();
    }

    ~Mutex() {
      int err = pthread_mutex_destroy(&native_);
      if(err != 0) {
        if(err == EBUSY) assert(0 && "mutex is busy!");
        if(err == EINVAL) assert(0 && "mutex is dead!");
        assert(0 && "mutex is screwed!");
      }
    }

    pthread_mutex_t* native() {
      return &native_;
    }

    void lock() {
      int err;
      if(cDebugLockGuard) {
        std::cout << "[[ " << pthread_self() << "   MLocking " << describe() << " ]]\n";
      }
      if((err = pthread_mutex_lock(&native_)) != 0) {
        if(err == EDEADLK) {
          std::cout << "Thread deadlock!\n";
        }

        assert(0);
      }
      if(cDebugLockGuard) {
        std::cout << "[[ " << pthread_self() << "    MLocked " << describe() << " ]]\n";
      }
    }

    Code try_lock() {
      int err = pthread_mutex_trylock(&native_);
      if(err != 0) {
        if(err == EBUSY) return cLockBusy;
        assert(0);
      }

      return cLocked;
    }

    Code unlock() {
      if(cDebugLockGuard) {
        std::cout << "[[ " << pthread_self() << "   MUnlocking " << describe() << " ]]\n";
      }
      int err = pthread_mutex_unlock(&native_);
      if(err != 0) {
        if(err == EPERM) return cNotYours;
        assert(0);
      }

      return cUnlocked;
    }

    std::string describe() {
      std::stringstream ss;
      ss << "Mutex ";
      ss << (void*)this;
      return ss.str();
    }
  };

  class Condition {
    pthread_cond_t native_;

  public:
    void init() {
      pthread_check(pthread_cond_init(&native_, NULL));
    }

    Condition() {
      init();
    }

    ~Condition() {
      pthread_check(pthread_cond_destroy(&native_));
    }

    void signal() {
      pthread_check(pthread_cond_signal(&native_));
    }

    void broadcast() {
      pthread_check(pthread_cond_broadcast(&native_));
    }

    void wait(Mutex& mutex) {
      if(cDebugLockGuard) {
        std::cout << "[[ " << pthread_self() << "   CUnlocking " << mutex.describe() << " ]]\n";
      }

      pthread_check(pthread_cond_wait(&native_, mutex.native()));

      if(cDebugLockGuard) {
        std::cout << "[[ " << pthread_self() << "   CLocked " << mutex.describe() << " ]]\n";
      }
    }

    Code wait_until(Mutex& mutex, const struct timespec* ts) {
      if(cDebugLockGuard) {
        std::cout << "[[ " << pthread_self() << "   CUnlocking " << mutex.describe() << " ]]\n";
      }

      int err = pthread_cond_timedwait(&native_, mutex.native(), ts);
      if(cDebugLockGuard) {
        std::cout << "[[ " << pthread_self() << "   CLocked " << mutex.describe() << " ]]\n";
      }

      if(err != 0) {
        if(err == ETIMEDOUT) {
          return cTimedOut;
        }

        switch(err) {
        case EINVAL:
          // This is not really correct, but it works for us:
          // We treat this error as ONLY ts being invalid, ie, it's for
          // a time in the past. Thus we can just say everything is ready.
          //
          // EINVAL can mean that both native_ and mutex.native() are invalid
          // too, but we've got no recourse if that is true.
          return cReady;
        default:
          std::cout << "Unknown failure from pthread_cond_timedwait!\n";
        }

        assert(0);
      }
      return cReady;
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

#ifdef HAVE_OSX_SPINLOCK

#include <libkern/OSAtomic.h>

namespace thread {

  class SpinLock {
  public: // Types
    typedef StackLockGuard<SpinLock> LockGuard;
    typedef StackUnlockGuard<SpinLock> UnlockGuard;

  private:
    OSSpinLock native_;

  public:
    SpinLock()
      : native_(0)
    {}

    void lock() {
      OSSpinLockLock(&native_);
    }

    void unlock() {
      OSSpinLockUnlock(&native_);
    }

    Code try_lock() {
      if(OSSpinLockTry(&native_)) {
        return cLockBusy;
      }

      return cLocked;
    }

    std::string describe() {
      std::stringstream ss;
      ss << "SpinLock ";
      ss << (void*)this;
      return ss.str();
    }
  };
};

#elif ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1))

// You can uncomment this to use compare_and_swap, but
// CAS doesn't have any fencing, so it's not known how they'll
// behave.
// #define USE_SYNC_CAS

namespace thread {
  class SpinLock {
  public: // Types
    typedef StackLockGuard<SpinLock> LockGuard;
    typedef StackUnlockGuard<SpinLock> UnlockGuard;

  private:
    int lock_;

  public:
    SpinLock()
      : lock_(1)
    {}

    void lock() {
#ifdef USE_SYNC_CAS
      while(!__sync_bool_compare_and_swap(&lock_, 1, 0));
      __sync_synchronize(); // HM?!
#else
      __sync_lock_test_and_set(&lock_, 1);
#endif
    }

    void unlock() {
#ifdef USE_SYNC_CAS
      lock_ = 1;
#else
      // See http://bugs.mysql.com/bug.php?id=34175 about using
      // __sync_lock_release
      __sync_lock_test_and_set(&lock_, 0);
#endif
    }

    Code try_lock() {
      if(__sync_bool_compare_and_swap(&lock_, 1, 0)) {
        return cLocked;
      }

      return cLockBusy;
    }

    std::string describe() {
      std::stringstream ss;
      ss << "SpinLock ";
      ss << (void*)this;
      return ss.str();
    }
  };
}

#else

namespace thread {
  typedef Mutex SpinLock;
};

#endif

#else

#error "No thread implementation defined"

#endif

#endif
