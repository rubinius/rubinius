#ifndef RBX_THREAD_NEXUS_HPP
#define RBX_THREAD_NEXUS_HPP

#include "bug.hpp"
#include "defines.hpp"

#include "util/thread.hpp"
#include "util/atomic.hpp"

#include <list>

#include <stdint.h>

namespace rubinius {
  namespace memory {
    class ManagedThread;
  }

  class SharedState;
  class State;
  class VM;

  typedef std::list<memory::ManagedThread*> ThreadList;

  class ThreadNexus {
    bool stop_;
    utilities::thread::SpinLock threads_lock_;
    utilities::thread::Mutex lock_;
    utilities::thread::Mutex sleep_lock_;
    utilities::thread::Mutex wait_mutex_;
    utilities::thread::Condition wait_condition_;
    ThreadList threads_;
    uint32_t thread_ids_;

  public:
    enum Phase {
      cStop       = 0x01,
      cManaged    = 0x02,
      cBlocking   = 0x04,
      cUnmanaged  = 0x81,
      cWaiting    = 0x82,
      cSleeping   = 0x84,
      cYielding   = 0x80
    };

    ThreadNexus()
      : stop_(false)
      , threads_lock_()
      , lock_(true)
      , sleep_lock_()
      , wait_mutex_()
      , wait_condition_()
      , threads_()
      , thread_ids_(0)
    { }

    ~ThreadNexus() {
      rubinius::bug("attempt to destroy ThreadNexus");
    }

    ThreadList* threads() {
      return &threads_;
    }

    utilities::thread::SpinLock& threads_lock() {
      return threads_lock_;
    }

    bool stop_p() {
      return stop_;
    }

    void set_stop() {
      stop_ = true;
      atomic::memory_barrier();
    }

    void unlock() {
      stop_ = false;
      wait_condition_.broadcast();
      lock_.unlock();
    }

    bool blocking_p(VM* vm);
    void blocking(VM* vm);
    bool sleeping_p(VM* vm);
    bool yielding_p(VM* vm);
    void yielding(VM* vm);
    bool serialized_p(VM* vm);

    uint64_t delay();
    void list_threads();
    void detect_lock_deadlock(uint64_t nanoseconds, VM* vm);
    void detect_halt_deadlock(uint64_t nanoseconds, VM* vm);

    VM* new_vm(SharedState* shared, const char* name = NULL);
    VM* new_vm_solo(SharedState* shared, const char* name = NULL);
    void add_vm(VM* vm);
    void delete_vm(VM* vm);

    void after_fork_child(STATE);

    void lock(VM* vm);
    void waiting_lock(VM* vm);
    void managed_lock(VM* vm);
    void sleep_lock(VM* vm);
    bool stop_lock(VM* vm);

    void wait_till_alone(VM* vm);
  };
}

#endif
