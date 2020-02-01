#ifndef RBX_THREAD_NEXUS_HPP
#define RBX_THREAD_NEXUS_HPP

#include "bug.hpp"
#include "defines.hpp"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>

#include <stdint.h>

namespace rubinius {
  class Machine;

  class ThreadState;

  typedef std::list<ThreadState*> ThreadList;

  class ThreadNexus {
    std::atomic<bool> stop_;
    std::atomic<uint32_t> halt_;
    std::atomic<uint32_t> lock_;

    std::mutex threads_mutex_;
    std::mutex halting_mutex_;
    std::mutex waiting_mutex_;
    std::condition_variable waiting_condition_;

    ThreadList threads_;
    uint32_t thread_ids_;

  public:
    const static uint64_t cLockLimit = 5000000000;

    enum Phase {
      eManaged    = 0x01,
      eUnmanaged  = 0x81,
      eWaiting    = 0x82,
    };

    const static int cYieldingPhase = 0x80;

    ThreadNexus()
      : stop_(false)
      , halt_(0)
      , lock_(0)
      , threads_mutex_()
      , halting_mutex_()
      , waiting_mutex_()
      , waiting_condition_()
      , threads_()
      , thread_ids_(0)
    { }

    ~ThreadNexus() = delete;

  public:
    ThreadList* threads() {
      return &threads_;
    }

    std::mutex& threads_mutex() {
      return threads_mutex_;
    }

    bool stop_p() {
      return stop_.load(std::memory_order_acquire);
    }

    bool set_stop() {
      stop_.store(true, std::memory_order_release);

      return stop_p();
    }

    void unset_stop() {
      stop_.store(false, std::memory_order_release);
    }

    bool halt_p() {
      return halt_.load(std::memory_order_acquire) != 0;
    }

    void set_halt(STATE, ThreadState* vm);

    void managed_phase(STATE, ThreadState* vm);
    bool try_managed_phase(STATE, ThreadState* vm);
    void unmanaged_phase(STATE, ThreadState* vm);
    void waiting_phase(STATE, ThreadState* vm);

    void set_managed(STATE, ThreadState* vm);

    void each_thread(std::function<void (ThreadState*)> process);

    bool valid_thread_p(STATE, unsigned int thread_id);

#ifdef RBX_GC_STACK_CHECK
    void check_stack(STATE, ThreadState* vm);
#endif

    bool yielding_p(ThreadState* vm);

    void yield(STATE, ThreadState* vm) {
      while(stop_p()) {
        waiting_phase(state, vm);

#ifdef RBX_GC_STACK_CHECK
        check_stack(state, vm);
#endif

        if(halt_p()) {
          std::lock_guard<std::mutex> lock(halting_mutex_);
        } else {
          std::unique_lock<std::mutex> lock(waiting_mutex_);
          waiting_condition_.wait(lock, [this]{ return !stop_p(); });
        }

        managed_phase(state, vm);
      }
    }

    uint64_t wait();
    void wait_for_all(STATE, ThreadState* vm);

    bool lock_owned_p(ThreadState* vm);

    bool try_lock(ThreadState* vm);
    bool try_lock_wait(STATE, ThreadState* vm);

    bool can_stop_p(STATE, ThreadState* vm);

    bool check_stop(STATE, ThreadState* vm) {
      if(!can_stop_p(state, vm)) return false;

      while(stop_p()) {
        yield(state, vm);

        return true;
      }

      return false;
    }

    void stop(STATE, ThreadState* vm) {
      while(set_stop()) {
        if(try_lock_wait(state, vm)) {
          wait_for_all(state, vm);

          return;
        }
      }
    }

    void halt(STATE, ThreadState* vm) {
      set_halt(state, vm);
      stop(state, vm);
      unset_stop();
      unlock(state, vm);
    }

    void lock(STATE, ThreadState* vm, std::function<void ()> process) {
      lock(state, vm);
      process();
      unlock(state, vm);
    }

    void lock(STATE, ThreadState* vm) {
      try_lock_wait(state, vm);
    }

    bool try_lock(STATE, ThreadState* vm, std::function<void ()> process) {
      if(try_lock(vm)) {
        process();
        unlock(state, vm);
        return true;
      } else {
        return false;
      }
    }

    void unlock(STATE, ThreadState* vm);

    void detect_deadlock(STATE, uint64_t nanoseconds);
    void detect_deadlock(STATE, uint64_t nanoseconds, ThreadState* vm);

    void list_threads();
    void list_threads(logger::PrintFunction function);

    ThreadState* thread_state(Machine* m, const char* name = NULL);
    void delete_vm(ThreadState* vm);

    void after_fork_child(STATE);
  };
}

#endif
