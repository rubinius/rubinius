#ifndef RBX_THREAD_NEXUS_HPP
#define RBX_THREAD_NEXUS_HPP

#include "bug.hpp"
#include "defines.hpp"

#include "util/thread.hpp"
#include "util/atomic.hpp"

#include <atomic>
#include <list>
#include <mutex>

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
    std::atomic<bool> stop_;
    std::mutex threads_mutex_;
    std::recursive_mutex phase_mutex_;
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
      , threads_mutex_()
      , phase_mutex_()
      , threads_()
      , thread_ids_(0)
    { }

    ~ThreadNexus() {
      rubinius::bug("attempt to destroy ThreadNexus");
    }

    ThreadList* threads() {
      return &threads_;
    }

    std::mutex& threads_mutex() {
      return threads_mutex_;
    }

    bool stop_p() {
      return stop_.load(std::memory_order_acquire);
    }

    void set_stop() {
      stop_.store(true, std::memory_order_release);
    }

    void unset_stop() {
      stop_.store(false, std::memory_order_release);
    }

    void blocking_phase(VM* vm);
    void managed_phase(VM* vm);
    void unmanaged_phase(VM* vm);
    void waiting_phase(VM* vm);

    bool blocking_p(VM* vm);
    bool yielding_p(VM* vm);

    void waiting_lock(VM* vm);

    bool try_lock(VM* vm) {
      if(!stop_p()) return false;

      waiting_lock(vm);

      // Assumption about stop_ may change while we progress.
      if(stop_p()) {
        if(try_checkpoint(vm)) {
          if(stop_p()) {
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
      phase_mutex_.lock();
      set_stop();
      checkpoint(vm);
    }

    void unlock() {
      stop_.store(false, std::memory_order_release);
      phase_mutex_.unlock();
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
