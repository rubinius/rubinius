#ifndef RBX_UTIL_THREAD_WINDOWS_HPP
#define RBX_UTIL_THREAD_WINDOWS_HPP

typedef intptr_t pthread_t;

namespace thread {
  template <typename T = void*>
  class ThreadData {

  public:
    ThreadData() {
    }

    ~ThreadData() {
    }

    T get() {
      return reinterpret_cast<T>(1);
    }

    void set(T val) {
    }
  };

  class Thread {
    intptr_t native_;
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

    size_t stack_size() {
      return stack_size_;
    }

    int run() {
      return 1;
    }

    virtual void perform() { }

    void detach() {
    }

    bool equal(Thread& other) {
      return false;
    }

    void join() {
    }

    bool in_self_p() {
      return false;
    }

    void cancel() {
    }

    void kill(int sig) {
    }

    int priority() {
      return 1;
    }

    bool set_priority(int priority) {
      return false;
    }

    bool delete_on_exit() {
      return delete_on_exit_;
    }

    void set_delete_on_exit() {
      delete_on_exit_ = true;
    }
  };

  class Mutex {
  public: // Types
    typedef StackLockGuard<Mutex> LockGuard;
    typedef StackUnlockGuard<Mutex> UnlockGuard;

  private:

  public:
    void init() {
    }

    Mutex() {
      init();
    }

    ~Mutex() {
    }

    intptr_t* native() {
      static intptr_t p = 0;
      return &p;
    }

    void lock() {
    }

    Code try_lock() {
      return cNotYours;
    }

    Code unlock() {
      return cNotYours;
    }

    std::string describe() {
      std::stringstream ss;
      ss << "Mutex ";
      ss << (void*)this;
      return ss.str();
    }
  };

  class Condition {

  public:
    void init() {
    }

    Condition() {
      init();
    }

    ~Condition() {
    }

    void signal() {
    }

    void broadcast() {
    }

    void wait(Mutex& mutex) {
    }

    Code wait_until(Mutex& mutex, const struct timespec* ts) {
      return cNotYours;
    }
  };

  typedef Mutex SpinLock;
}

#endif
