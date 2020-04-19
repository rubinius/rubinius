#ifndef RBX_THREAD_NEXUS_HPP
#define RBX_THREAD_NEXUS_HPP

#include "bug.hpp"
#include "defines.hpp"
#include "logger.hpp"
#include "machine.hpp"

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
    std::atomic<uint32_t> lock_;

    std::mutex threads_mutex_;
    std::mutex& waiting_mutex_;
    std::condition_variable& waiting_condition_;

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

    ThreadNexus(std::mutex& waiting, std::condition_variable& waiting_cond)
      : stop_(false)
      , lock_(0)
      , threads_mutex_()
      , waiting_mutex_(waiting)
      , waiting_condition_(waiting_cond)
      , threads_()
      , thread_ids_(0)
    { }

    virtual ~ThreadNexus() { }

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
      return Machine::_halting_.load(std::memory_order_acquire) != 0;
    }

    void set_halt(STATE, ThreadState* thread_state);

    void managed_phase(STATE, ThreadState* thread_state);
    bool try_managed_phase(STATE, ThreadState* thread_state);
    void unmanaged_phase(STATE, ThreadState* thread_state);
    void waiting_phase(STATE, ThreadState* thread_state);

    void set_managed(STATE, ThreadState* thread_state);

    void each_thread(std::function<void (ThreadState*)> process);

    bool valid_thread_p(STATE, unsigned int thread_id);

#ifdef RBX_GC_STACK_CHECK
    void check_stack(STATE, ThreadState* thread_state);
#endif

    bool yielding_p(ThreadState* thread_state);

    void yield(STATE, ThreadState* thread_state);

    uint64_t wait();
    void wait_for_all(STATE);

    bool lock_owned_p(ThreadState* thread_state);

    bool try_lock(ThreadState* thread_state);
    bool try_lock_wait(STATE, ThreadState* thread_state);

    bool can_stop_p(STATE, ThreadState* thread_state);

    bool check_stop(STATE, ThreadState* thread_state) {
      if(!can_stop_p(state, thread_state)) return false;

      while(stop_p()) {
        yield(state, thread_state);

        return true;
      }

      return false;
    }

    void stop(STATE, ThreadState* thread_state) {
      while(set_stop()) {
        if(try_lock_wait(state, thread_state)) {
          wait_for_all(state);

          return;
        }
      }
    }

    void halt(STATE, ThreadState* thread_state) {
      set_halt(state, thread_state);
      stop(state, thread_state);
      unset_stop();
      unlock(state, thread_state);
    }

    void lock(STATE, ThreadState* thread_state, std::function<void ()> process) {
      lock(state, thread_state);
      process();
      unlock(state, thread_state);
    }

    void lock(STATE, ThreadState* thread_state) {
      try_lock_wait(state, thread_state);
    }

    bool try_lock(STATE, ThreadState* thread_state, std::function<void ()> process) {
      if(try_lock(thread_state)) {
        process();
        unlock(state, thread_state);
        return true;
      } else {
        return false;
      }
    }

    void unlock(STATE, ThreadState* thread_state);

    void detect_deadlock(STATE, uint64_t nanoseconds);
    void detect_deadlock(STATE, uint64_t nanoseconds, ThreadState* thread_state);

    void list_threads();
    void list_threads(logger::PrintFunction function);

    ThreadState* create_thread_state(Machine* m, const char* name = NULL);
    void remove_thread_state(ThreadState* thread_state);

    void after_fork_child(STATE);
  };
}

#endif
