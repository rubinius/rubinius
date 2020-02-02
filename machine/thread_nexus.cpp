#include "environment.hpp"
#include "logger.hpp"
#include "machine.hpp"
#include "thread_nexus.hpp"
#include "thread_phase.hpp"

#include "class/thread.hpp"

#include "memory/collector.hpp"

#include "util/atomic.hpp"

#include "diagnostics/timing.hpp"

#include <chrono>
#include <cxxabi.h>
#include <ostream>
#include <string>
#include <thread>
#include <time.h>

#ifdef USE_EXECINFO
#include <execinfo.h>
#endif

namespace rubinius {
  void ThreadNexus::set_halt(STATE, ThreadState* vm) {
    if(!halting_mutex_.try_lock()) {
      std::ostringstream msg;

      msg << "halting mutex is already locked: id: " << vm->thread_id();

      Exception::raise_assertion_error(state, msg.str().c_str());
    }

    halt_.store(vm->thread_id(), std::memory_order_release);
  }

  void ThreadNexus::managed_phase(STATE, ThreadState* vm) {
    if(halt_ && halt_ != vm->thread_id()) {
      halting_mutex_.lock();
    }

    if(can_stop_p(state, vm)) {
      lock(state, vm, [vm]{ vm->set_thread_phase(eManaged); });
    } else {
      // We already own the lock
      vm->set_thread_phase(eManaged);
    }
  }


  bool ThreadNexus::try_managed_phase(STATE, ThreadState* vm) {
    if(halt_ && halt_ != vm->thread_id()) {
      halting_mutex_.lock();
    }

    return try_lock(state, vm, [vm]{ vm->set_thread_phase(eManaged); });
  }

  void ThreadNexus::unmanaged_phase(STATE, ThreadState* vm) {
    vm->set_thread_phase(eUnmanaged);
  }

  void ThreadNexus::waiting_phase(STATE, ThreadState* vm) {
    if(!can_stop_p(state, vm)) {
      std::ostringstream msg;

      msg << "waiting while holding process-critical lock: id: "
          << vm->thread_id();

      Exception::raise_assertion_error(state, msg.str().c_str());
    }

    vm->set_thread_phase(eWaiting);
  }

  // Only to be used when holding the ThreadNexus lock.
  void ThreadNexus::set_managed(STATE, ThreadState* vm) {
    vm->set_thread_phase(eManaged);
  }

  bool ThreadNexus::can_stop_p(STATE, ThreadState* vm) {
    return lock_ != vm->thread_id();
  }

  void ThreadNexus::unlock(STATE, ThreadState* vm) {
    if(can_stop_p(state, vm)) {
      std::ostringstream msg;

      msg << "process-critical lock being unlocked by the wrong Thread: id: "
          << vm->thread_id();

      Exception::raise_assertion_error(state, msg.str().c_str());
    }

    waiting_condition_.notify_all();
    lock_ = 0;
  }

  bool ThreadNexus::yielding_p(ThreadState* vm) {
    int phase = static_cast<int>(vm->thread_phase());

    return (phase & cYieldingPhase) == cYieldingPhase;
  }

  ThreadState* ThreadNexus::thread_state(Machine* m, const char* name) {
    std::lock_guard<std::mutex> guard(threads_mutex_);

    uint32_t max_id = thread_ids_;
    uint32_t id = ++thread_ids_;

    if(id < max_id) {
      rubinius::bug("exceeded maximum number of threads");
    }

    ThreadState* vm = new ThreadState(id, m, name);

    threads_.push_back(vm);

    return vm;
  }

  void ThreadNexus::delete_vm(ThreadState* vm) {
    std::lock_guard<std::mutex> guard(threads_mutex_);

    threads_.remove(vm);
  }

  void ThreadNexus::after_fork_child(STATE) {
    new(&threads_mutex_) std::mutex;

    while(!threads_.empty()) {
      ThreadState* ts = threads_.back();
      threads_.pop_back();

      if(!ts) continue;

      switch(ts->kind()) {
        case ThreadState::eThread: {
          if(Thread* thread = ts->thread()) {
            if(!thread->nil_p()) {
              if(ts == state) {
                thread->current_fiber(state, thread->fiber());
                continue;
              }

              thread->stopped();
            }
          }

          ts->reset_parked();
          ts->set_zombie();

          break;
        }
        case ThreadState::eFiber: {
          if(Fiber* fiber = ts->fiber()) {
            fiber->status(Fiber::eDead);
            ts->set_canceled();
            ts->set_zombie();
          }

          break;
        }
        case ThreadState::eSystem:
          ThreadState::discard(state, ts);
          break;
      }
    }

    threads_.push_back(state);
  }

  static const char* phase_name(ThreadState* vm) {
    switch(vm->thread_phase()) {
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
      if(ThreadState* other_vm = (*i)) {
        function("thread %d: %s, %s",
            other_vm->thread_id(), other_vm->name().c_str(), phase_name(other_vm));
      }
    }
  }

  void ThreadNexus::detect_deadlock(STATE, uint64_t nanoseconds, ThreadState* vm) {
    if(nanoseconds > ThreadNexus::cLockLimit) {
      logger::error("thread nexus: thread will not yield: %s, %s",
          vm->name().c_str(), phase_name(vm));

      list_threads(logger::error);

      std::ostringstream msg;
      msg << "thread will not yield: " << vm->name().c_str() << phase_name(vm);

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

  void ThreadNexus::each_thread(std::function<void (ThreadState* vm)> process) {
    std::lock_guard<std::mutex> guard(threads_mutex_);
    // LockWaiting<std::mutex> guard(state, threads_mutex_);

    for(auto vm : threads_) {
      process(reinterpret_cast<ThreadState*>(vm));
    }
  }

  bool ThreadNexus::valid_thread_p(STATE, unsigned int thread_id) {
    bool valid = false;

    each_thread([&](ThreadState* vm) {
        if(thread_id == vm->thread_id()) valid = true;
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

  void ThreadNexus::wait_for_all(STATE, ThreadState* vm) {
    std::lock_guard<std::mutex> guard(threads_mutex_);

    uint64_t limit = 0;

    set_managed(state, vm);

    for(ThreadList::iterator i = threads_.begin();
           i != threads_.end();
           ++i)
    {
      if(ThreadState* other_vm = (*i)) {
        if(vm == other_vm || yielding_p(other_vm)) continue;

        while(true) {
          if(yielding_p(other_vm)) {
            break;
          }

          limit += wait();

          detect_deadlock(state, limit, other_vm);
        }
      }
    }
  }

  bool ThreadNexus::lock_owned_p(ThreadState* vm) {
    return lock_ == vm->thread_id();
  }

  bool ThreadNexus::try_lock(ThreadState* vm) {
    uint32_t id = 0;

    return lock_.compare_exchange_strong(id, vm->thread_id());
  }

  bool ThreadNexus::try_lock_wait(STATE, ThreadState* vm) {
    uint64_t limit = 0;

    while(!try_lock(vm)) {
      if(state->collector()->collect_requested_p()) {
        yield(state, vm);
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

  void ThreadNexus::check_stack(STATE, ThreadState* vm) {
    void* callstack[RBX_ABORT_CALLSTACK_SIZE];
    char symbol[RBX_ABORT_SYMBOL_SIZE];

    int i, frames = backtrace(callstack, RBX_ABORT_CALLSTACK_SIZE);
    char** symbols = backtrace_symbols(callstack, frames);

    logger::debug("Backtrace for %s: %s", vm->kind_name(), vm->name().c_str());
    for(i = 0; i < frames; i++) {
      logger::debug("%s", demangle(symbols[i], symbol, RBX_ABORT_SYMBOL_SIZE));
    }

    free(symbols);
  }
#endif
}
