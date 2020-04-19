#include "environment.hpp"
#include "logger.hpp"
#include "machine.hpp"
#include "thread_nexus.hpp"
#include "thread_phase.hpp"

#include "class/thread.hpp"

#include "memory/collector.hpp"

#include "diagnostics/timing.hpp"

#include <chrono>
#include <cxxabi.h>
#include <sstream>
#include <string>
#include <thread>
#include <time.h>

#ifdef USE_EXECINFO
#include <execinfo.h>
#endif

namespace rubinius {
  void ThreadNexus::set_halt(STATE, ThreadState* thread_state) {
    if(Machine::_halting_ && Machine::_halting_ == thread_state->thread_id()) {
      std::ostringstream msg;

      msg << "halting mutex is already locked: id: " << thread_state->thread_id();

      Exception::raise_assertion_error(state, msg.str().c_str());
    }

    Machine::_halting_.store(thread_state->thread_id(), std::memory_order_release);
  }

  void ThreadNexus::managed_phase(STATE, ThreadState* thread_state) {
    if(Machine::_halting_ && Machine::_halting_ != thread_state->thread_id()) {
      thread_state->halt_thread();
    }

    if(can_stop_p(state, thread_state)) {
      lock(state, thread_state,
          [thread_state]{ thread_state->set_thread_phase(eManaged); });
    } else {
      // We already own the lock
      thread_state->set_thread_phase(eManaged);
    }
  }


  bool ThreadNexus::try_managed_phase(STATE, ThreadState* thread_state) {
    if(Machine::_halting_ && Machine::_halting_ != thread_state->thread_id()) {
      thread_state->halt_thread();
    }

    return try_lock(state, thread_state,
        [thread_state]{ thread_state->set_thread_phase(eManaged); });
  }

  void ThreadNexus::unmanaged_phase(STATE, ThreadState* thread_state) {
    thread_state->set_thread_phase(eUnmanaged);
  }

  void ThreadNexus::waiting_phase(STATE, ThreadState* thread_state) {
    if(!can_stop_p(state, thread_state)) {
      std::ostringstream msg;

      msg << "waiting while holding process-critical lock: id: "
          << thread_state->thread_id();

      Exception::raise_assertion_error(state, msg.str().c_str());
    }

    thread_state->set_thread_phase(eWaiting);
  }

  // Only to be used when holding the ThreadNexus lock.
  void ThreadNexus::set_managed(STATE, ThreadState* thread_state) {
    thread_state->set_thread_phase(eManaged);
  }

  bool ThreadNexus::can_stop_p(STATE, ThreadState* thread_state) {
    return lock_ != thread_state->thread_id();
  }

  void ThreadNexus::yield(STATE, ThreadState* thread_state) {
    while(stop_p()) {
      waiting_phase(state, thread_state);

#ifdef RBX_GC_STACK_CHECK
      check_stack(state, thread_state);
#endif

      if(halt_p()) {
        thread_state->halt_thread();
        // std::lock_guard<std::mutex> lock(halting_mutex_);
      } else {
        std::unique_lock<std::mutex> lock(waiting_mutex_);
        waiting_condition_.wait(lock, [this]{ return !stop_p(); });
      }

      managed_phase(state, thread_state);
    }
  }

  void ThreadNexus::unlock(STATE, ThreadState* thread_state) {
    if(can_stop_p(state, thread_state)) {
      std::ostringstream msg;

      msg << "process-critical lock being unlocked by the wrong Thread: id: "
          << thread_state->thread_id();

      Exception::raise_assertion_error(state, msg.str().c_str());
    }

    waiting_condition_.notify_all();
    lock_ = 0;
  }

  bool ThreadNexus::yielding_p(ThreadState* thread_state) {
    int phase = static_cast<int>(thread_state->thread_phase());

    return (phase & cYieldingPhase) == cYieldingPhase;
  }

  ThreadState* ThreadNexus::create_thread_state(Machine* m, const char* name) {
    std::lock_guard<std::mutex> guard(threads_mutex_);

    uint32_t max_id = thread_ids_;
    uint32_t id = ++thread_ids_;

    if(id < max_id) {
      rubinius::bug("exceeded maximum number of threads");
    }

    ThreadState* thread_state = new ThreadState(id, m, name);

    threads_.push_back(thread_state);

    return thread_state;
  }

  void ThreadNexus::remove_thread_state(ThreadState* thread_state) {
    std::lock_guard<std::mutex> guard(threads_mutex_);

    threads_.remove(thread_state);
  }

  void ThreadNexus::after_fork_child(STATE) {
    threads_mutex_.try_lock();
    threads_mutex_.unlock();

    for(auto i = threads_.begin(); i != threads_.end();) {
      ThreadState* thread_state = *i;

      switch(thread_state->kind()) {
        case ThreadState::eThread: {
          if(Thread* thread = thread_state->thread()) {
            if(!thread->nil_p()) {
              if(thread_state == state) {
                thread->current_fiber(state, thread->fiber());
                ++i;
                continue;
              } else {
                thread_state->set_thread_dead();
                i = threads_.erase(i);
              }
            }
          }

          break;
        }
        case ThreadState::eFiber: {
          if(Fiber* fiber = thread_state->fiber()) {
            fiber->status(Fiber::eDead);
            thread_state->set_canceled();
          }

          thread_state->set_thread_dead();
          i = threads_.erase(i);

          break;
        }
        case ThreadState::eSystem:
          ++i;
          break;
      }
    }
  }

  static const char* phase_name(ThreadState* thread_state) {
    switch(thread_state->thread_phase()) {
      case ThreadNexus::eManaged:
        return "eManaged";
      case ThreadNexus::eUnmanaged:
        return "eUnmanaged";
      case ThreadNexus::eWaiting:
        return "eWaiting";
    }

    return "cUnknown";
  }

  void ThreadNexus::list_threads() {
    list_threads(logger::error);
  }

  void ThreadNexus::list_threads(logger::PrintFunction function) {
    for(ThreadList::iterator i = threads_.begin();
           i != threads_.end();
           ++i)
    {
      if(ThreadState* thread_state = *i) {
        function("thread %d: %s, %s",
            thread_state->thread_id(), thread_state->name().c_str(), phase_name(thread_state));
      }
    }
  }

  void ThreadNexus::detect_deadlock(STATE, uint64_t nanoseconds, ThreadState* thread_state) {
    if(nanoseconds > ThreadNexus::cLockLimit) {
      logger::error("thread nexus: thread will not yield: %s, %s",
          thread_state->name().c_str(), phase_name(thread_state));

      list_threads(logger::error);

      std::ostringstream msg;
      msg << "thread will not yield: " << thread_state->name().c_str() << phase_name(thread_state);

      Exception::raise_deadlock_error(state, msg.str().c_str());
    }
  }

  void ThreadNexus::detect_deadlock(STATE, uint64_t nanoseconds) {
    if(nanoseconds > ThreadNexus::cLockLimit) {
      logger::error("thread nexus: unable to lock, possible deadlock");

      list_threads(logger::error);

      Exception::raise_deadlock_error(state,
          "thread nexus: unable to lock, possible deadlock");
    }
  }

  void ThreadNexus::each_thread(std::function<void (ThreadState* thread_state)> process) {
    std::lock_guard<std::mutex> guard(threads_mutex_);
    // LockWaiting<std::mutex> guard(state, threads_mutex_);

    for(auto thread_state : threads_) {
      process(reinterpret_cast<ThreadState*>(thread_state));
    }
  }

  bool ThreadNexus::valid_thread_p(STATE, unsigned int thread_id) {
    bool valid = false;

    each_thread([&](ThreadState* thread_state) {
        if(thread_id == thread_state->thread_id()) valid = true;
      });

    return valid;
  }

  uint64_t ThreadNexus::wait() {
    static int i = 0;
    static int delay[] = {
      133, 464, 254, 306, 549, 287, 358, 638, 496, 81,
      472, 288, 131, 31, 435, 258, 221, 73, 537, 854
    };
    static int modulo = sizeof(delay) / sizeof(int);

    uint64_t ns = delay[i++ % modulo];

    std::this_thread::sleep_for(std::chrono::nanoseconds(ns));

    return ns;
  }

  void ThreadNexus::wait_for_all(STATE) {
    std::lock_guard<std::mutex> guard(threads_mutex_);

    uint64_t limit = 0;

    set_managed(state, state);

    for(ThreadList::iterator i = threads_.begin();
           i != threads_.end();
           ++i)
    {
      if(ThreadState* thread_state = *i) {
        if(state == thread_state || yielding_p(thread_state)) continue;

        while(true) {
          if(yielding_p(thread_state)) {
            break;
          }

          limit += wait();

          detect_deadlock(state, limit, thread_state);
        }
      }
    }
  }

  bool ThreadNexus::lock_owned_p(ThreadState* thread_state) {
    return lock_ == thread_state->thread_id();
  }

  bool ThreadNexus::try_lock(ThreadState* thread_state) {
    uint32_t id = 0;

    return lock_.compare_exchange_strong(id, thread_state->thread_id());
  }

  bool ThreadNexus::try_lock_wait(STATE, ThreadState* thread_state) {
    uint64_t limit = 0;

    while(!try_lock(thread_state)) {
      if(state->collector()->collect_requested_p()) {
        yield(state, thread_state);
      }

      limit += wait();

      detect_deadlock(state, limit);
    }

    return true;
  }

  static char* demangle(char* symbol, char* result, size_t size) {
    const char* pos = strstr(symbol, " _Z");

    if(pos) {
      size_t sz = 0;
      char *cpp_name = 0;
      char* name = strdup(pos + 1);
      char* end = strstr(name, " + ");
      *end = 0;

      int status;
      if((cpp_name = abi::__cxa_demangle(name, cpp_name, &sz, &status))) {
        if(!status) {
          snprintf(result, size, "%.*s %s %s", int(pos - symbol), symbol, cpp_name, ++end);
        }
        free(cpp_name);
      }

      free(name);
    } else {
      strcpy(result, symbol);
    }

    return result;
  }

#ifdef RBX_GC_STACK_CHECK
#define RBX_ABORT_CALLSTACK_SIZE    128
#define RBX_ABORT_SYMBOL_SIZE       512

  void ThreadNexus::check_stack(STATE, ThreadState* thread_state) {
    void* callstack[RBX_ABORT_CALLSTACK_SIZE];
    char symbol[RBX_ABORT_SYMBOL_SIZE];

    int i, frames = backtrace(callstack, RBX_ABORT_CALLSTACK_SIZE);
    char** symbols = backtrace_symbols(callstack, frames);

    logger::debug("Backtrace for %s: %s",
        thread_state->kind_name(), thread_state->name().c_str());
    for(i = 0; i < frames; i++) {
      logger::debug("%s", demangle(symbols[i], symbol, RBX_ABORT_SYMBOL_SIZE));
    }

    free(symbols);
  }
#endif
}
