#ifndef RBX_UTIL_THREAD_HPP
#define RBX_UTIL_THREAD_HPP

#define USE_PTHREADS

#ifdef USE_PTHREADS
#include <pthread.h>
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
      assert(pthread_key_create(&native_, NULL) == 0);
    }

    ~ThreadData() {
      assert(pthread_key_delete(native_) == 0);
    }

    T get() {
      return reinterpret_cast<T>(pthread_getspecific(native_));
    }

    void set(T val) {
      assert(pthread_setspecific(native_, reinterpret_cast<void*>(val)) == 0);
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

    void run() {
      pthread_attr_t attrs;
      pthread_attr_init(&attrs);
      if(stack_size_) {
        pthread_attr_setstacksize(&attrs, stack_size_);
      }

      assert(pthread_create(&native_, &attrs, trampoline, (void*)this) == 0);
    }

    virtual void perform() { }

    void detach() {
      assert(pthread_detach(native_) == 0);
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
        }

        assert(0);
      }
    }

    void cancel() {
      assert(pthread_cancel(native_) == 0);
    }

    void kill(int sig) {
      assert(pthread_kill(native_, sig) == 0);
    }

    int priority() {
      int _policy;
      struct sched_param params;

      assert(pthread_getschedparam(native_, &_policy, &params) == 0);

      return params.sched_priority;
    }

    bool set_priority(int priority) {
      int _policy;
      struct sched_param params;

      assert(pthread_getschedparam(native_, &_policy, &params) == 0);
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
    Mutex() {
      assert(pthread_mutex_init(&native_, NULL) == 0);
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
    bool ready_;

  public:
    Condition()
      : ready_(false)
    {
      assert(pthread_cond_init(&native_, NULL) == 0);
    }

    ~Condition() {
      assert(pthread_cond_destroy(&native_) == 0);
    }

    void reset() {
      ready_ = false;
    }

    void signal() {
      ready_ = true;
      assert(pthread_cond_signal(&native_) == 0);
    }

    void broadcast() {
      ready_ = true;
      assert(pthread_cond_broadcast(&native_) == 0);
    }

    void wait(Mutex& mutex) {
      if(cDebugLockGuard) {
        std::cout << "[[ " << pthread_self() << "   CUnlocking " << mutex.describe() << " ]]\n";
      }
      while(!ready_) {
        assert(pthread_cond_wait(&native_, mutex.native()) == 0);
      }

      if(cDebugLockGuard) {
        std::cout << "[[ " << pthread_self() << "   CLocked " << mutex.describe() << " ]]\n";
      }
    }

    Code wait_until(Mutex& mutex, const struct timespec* ts) {
      if(cDebugLockGuard) {
        std::cout << "[[ " << pthread_self() << "   CUnlocking " << mutex.describe() << " ]]\n";
      }

      while(!ready_) {
        int err = pthread_cond_timedwait(&native_, mutex.native(), ts);
        if(cDebugLockGuard) {
          std::cout << "[[ " << pthread_self() << "   CLocked " << mutex.describe() << " ]]\n";
        }

        if(err != 0) {
          if(err == ETIMEDOUT) {
            if(ready_) return cReady;
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
      }

      return cReady;
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

#else

namespace thread {
  typedef Mutex SpinLock;
};

#endif

#else

#error "No thread implementation defined"

#endif

#endif
