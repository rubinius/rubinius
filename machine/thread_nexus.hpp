#ifndef RBX_THREAD_NEXUS_HPP
#define RBX_THREAD_NEXUS_HPP

#include "bug.hpp"
#include "defines.hpp"

#include "util/thread.hpp"
#include "util/atomic.hpp"

#include <atomic>
#include <condition_variable>
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
    std::mutex process_mutex_;
    std::mutex threads_mutex_;
    std::mutex waiting_mutex_;
    std::condition_variable waiting_condition_;

    std::atomic<uint32_t> phase_flag_;

    ThreadList threads_;
    uint32_t thread_ids_;

    const static uint64_t lock_limit = 5000000000;

  public:
    enum Phase {
      eManaged    = 0x01,
      eBlocking   = 0x02,
      eUnmanaged  = 0x81,
      eWaiting    = 0x82,
    };

    enum LockStatus {
      eNotLocked  = 0x0,
      eHeldLock   = 0x1,
      eLocked     = 0x2,
    };

    const static int cYieldingPhase = 0x80;

    ThreadNexus()
      : stop_(false)
      , process_mutex_()
      , threads_mutex_()
      , waiting_mutex_()
      , waiting_condition_()
      , phase_flag_(0)
      , threads_()
      , thread_ids_(0)
    { }

    ~ThreadNexus() {
      rubinius::bug("attempt to destroy ThreadNexus");
    }

  private:
    LockStatus to_lock_status(bool flag) {
      return flag ? eHeldLock : eLocked;
    }

  public:
    std::mutex& process_mutex() {
      return process_mutex_;
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

    void yield(VM* vm) {
      while(stop_p()) {
        waiting_phase(vm);

        {
          std::unique_lock<std::mutex> lk(waiting_mutex_);
          waiting_condition_.wait(lk,
              [this]{ return !stop_.load(std::memory_order_acquire); });
        }

        managed_phase(vm);
      }
    }

    bool waiting_lock(VM* vm);
    LockStatus fork_lock(VM* vm);
    void fork_unlock(LockStatus status);

    LockStatus try_lock(VM* vm) {
      while(stop_p()) {
        bool held = waiting_lock(vm);

        // Assumption about stop_ may change while we progress.
        if(stop_p()) {
          if(try_checkpoint(vm)) {
            if(stop_p()) {
              unset_stop();
              return to_lock_status(held);
            }
          }
        }

        // Either we're not stop_'ing or something blocked us from serializing.
        if(!held) unlock();
      }

      return eNotLocked;
    }

    LockStatus lock(VM* vm) {
      bool held = waiting_lock(vm);
      set_stop();
      checkpoint(vm);
      unset_stop();
      return to_lock_status(held);
    }

    void unlock() {
      waiting_condition_.notify_all();
      phase_flag_ = 0;
    }

    bool try_checkpoint(VM* vm);
    void checkpoint(VM* vm);

    uint64_t delay();
    void detect_deadlock(uint64_t nanoseconds, uint64_t limit, VM* vm);
    void detect_deadlock(uint64_t nanoseconds, uint64_t limit);

    void list_threads();

    VM* new_vm(SharedState* shared, const char* name = NULL);
    void delete_vm(VM* vm);

    void after_fork_child(STATE);

    void restore_phase(VM* vm, Phase phase);
  };
}

#endif
