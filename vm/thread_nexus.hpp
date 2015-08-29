#ifndef RBX_THREAD_NEXUS_HPP
#define RBX_THREAD_NEXUS_HPP

#include "bug.hpp"
#include "defines.hpp"

#include "util/thread.hpp"
#include "util/atomic.hpp"

#include <list>

#include <stdint.h>

namespace rubinius {
  class ManagedThread;
  class SharedState;
  class State;
  class VM;

  typedef std::list<ManagedThread*> ThreadList;

  class ThreadNexus {
    bool stop_;
    utilities::thread::SpinLock threads_lock_;
    utilities::thread::Mutex lock_;
    utilities::thread::Mutex wait_mutex_;
    utilities::thread::Condition wait_condition_;
    ThreadList threads_;
    uint32_t thread_ids_;

  public:
    enum Phase {
      cStop       = 0x01,
      cManaged    = 0x02,
      cUnmanaged  = 0x81,
      cWaiting    = 0x82,
      cYielding   = 0x80
    };

    ThreadNexus()
      : stop_(false)
      , threads_lock_()
      , lock_(true)
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

    bool yielding_p(VM* vm);
    void yielding(VM* vm);
    bool locking(VM* vm);
    VM* new_vm(SharedState* shared, const char* name = NULL);
    VM* new_vm_solo(SharedState* shared, const char* name = NULL);
    void add_vm(VM* vm);
    void delete_vm(VM* vm);
    void after_fork_child(STATE);
    void become_managed(VM* vm);
    bool lock_or_yield(VM* vm);
    bool lock_or_wait(VM* vm);
  };
}

#endif
