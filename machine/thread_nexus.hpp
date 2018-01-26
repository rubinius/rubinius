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
  namespace memory {
    class ManagedThread;
  }

  class SharedState;
  class State;
  class VM;

  typedef std::list<memory::ManagedThread*> ThreadList;

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

    void set_halt(STATE, VM* vm);

    void managed_phase(STATE, VM* vm);
    void unmanaged_phase(STATE, VM* vm);
    void waiting_phase(STATE, VM* vm);

    void set_managed(STATE, VM* vm);

    bool yielding_p(VM* vm);

    void yield(STATE, VM* vm) {
      while(stop_p()) {
        waiting_phase(state, vm);

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
    void wait_for_all(STATE, VM* vm);

    bool try_lock(VM* vm);
    bool try_lock_wait(STATE, VM* vm);

    void check_stop(STATE, VM* vm, std::function<void ()> process) {
      while(stop_p()) {
        if(try_lock(vm)) {
          wait_for_all(state, vm);

          process();

          unset_stop();
          unlock(state, vm);
        } else {
          yield(state, vm);
        }
      }
    }

    void stop(STATE, VM* vm) {
      while(set_stop()) {
        if(try_lock_wait(state, vm)) {
          wait_for_all(state, vm);

          return;
        }
      }
    }

    void halt(STATE, VM* vm) {
      set_halt(state, vm);
      stop(state, vm);
      unset_stop();
      unlock(state, vm);
    }

    void lock(STATE, VM* vm, std::function<void ()> process) {
      lock(state, vm);
      process();
      unlock(state, vm);
    }

    void lock(STATE, VM* vm) {
      try_lock_wait(state, vm);
    }

    void unlock(STATE, VM* vm);

    void detect_deadlock(STATE, uint64_t nanoseconds);
    void detect_deadlock(STATE, uint64_t nanoseconds, VM* vm);

    void list_threads();
    void list_threads(logger::PrintFunction function);

    VM* new_vm(SharedState* shared, const char* name = NULL);
    void delete_vm(VM* vm);

    void after_fork_child(STATE);
  };
}

#endif
