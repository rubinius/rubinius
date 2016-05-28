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
    uint32_t halt_;
    utilities::thread::SpinLock threads_lock_;
    utilities::thread::Mutex phase_lock_;
    utilities::thread::Mutex halt_lock_;
    ThreadList threads_;
    uint32_t thread_ids_;

    const static uint64_t try_lock_limit = 500000000;
    const static uint64_t lock_limit = 5000000000;

  public:
    enum Phase {
      eManaged    = 0x01,
      eBlocking   = 0x02,
      eUnmanaged  = 0x81,
      eWaiting    = 0x82,
    };

    const static int cYieldingPhase = 0x80;

    ThreadNexus()
      : stop_(false)
      , halt_(0)
      , threads_lock_()
      , phase_lock_(true)
      , halt_lock_(true)
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

    void set_halt(VM* vm);

    void blocking_phase(VM* vm);
    void managed_phase(VM* vm);
    void unmanaged_phase(VM* vm);
    void waiting_phase(VM* vm);

    bool blocking_p(VM* vm);
    bool yielding_p(VM* vm);

    bool try_lock(VM* vm) {
      if(!stop_) return false;

      waiting_lock(vm);

      // Assumption about stop_ may change while we progress.
      if(stop_) {
        if(try_checkpoint(vm)) {
          if(stop_) {
            return true;
          }
        }
      }

      // Either we're not stop_'ing or something blocked us from serializing.
      unlock();
      return false;
    }

    void lock(VM* vm) {
      waiting_lock(vm);
      set_stop();
      checkpoint(vm);
    }

    void halt_lock(VM* vm) {
      waiting_lock(vm);

      halt_lock_.lock();

      set_halt(vm);
      set_stop();

      checkpoint(vm);
    }

    void waiting_lock(VM* vm);

    void unlock() {
      stop_ = false;
      phase_lock_.unlock();
    }

    bool try_checkpoint(VM* vm);
    void checkpoint(VM* vm);

    uint64_t delay();
    void detect_deadlock(uint64_t nanoseconds, uint64_t limit, VM* vm);

    void list_threads();

    VM* new_vm(SharedState* shared, const char* name = NULL);
    void delete_vm(VM* vm);

    void after_fork_child(STATE);

    void restore_phase(VM* vm, Phase phase);
  };
}

#endif
