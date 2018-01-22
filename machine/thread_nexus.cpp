#include "logger.hpp"
#include "shared_state.hpp"
#include "thread_nexus.hpp"
#include "vm.hpp"

#include "class/thread.hpp"

#include "memory/managed.hpp"

#include "util/atomic.hpp"

#include "instruments/timing.hpp"

#include <chrono>
#include <ostream>
#include <string>
#include <thread>
#include <time.h>

namespace rubinius {
  void ThreadNexus::managed_phase(STATE, VM* vm) {
    lock(state, vm, [vm]{ vm->set_thread_phase(eManaged); });
  }

  void ThreadNexus::unmanaged_phase(STATE, VM* vm) {
    vm->set_thread_phase(eUnmanaged);
  }

  void ThreadNexus::waiting_phase(STATE, VM* vm) {
    if(lock_ == vm->thread_id()) {
      std::ostringstream msg;

      msg << "waiting while holding process-critical lock: id: "
          << vm->thread_id();

      Exception::raise_assertion_error(state, msg.str().c_str());
    }

    vm->set_thread_phase(eWaiting);
  }

  // Only to be used when holding the ThreadNexus lock.
  void ThreadNexus::set_managed(STATE, VM* vm) {
    vm->set_thread_phase(eManaged);
  }

  void ThreadNexus::unlock(STATE, VM* vm) {
    if(lock_ != vm->thread_id()) {
      std::ostringstream msg;

      msg << "process-critical lock being unlocked by the wrong Thread: id: "
          << vm->thread_id();

      Exception::raise_assertion_error(state, msg.str().c_str());
    }

    waiting_condition_.notify_all();
    lock_ = 0;
  }

  bool ThreadNexus::yielding_p(VM* vm) {
    int phase = static_cast<int>(vm->thread_phase());

    return (phase & cYieldingPhase) == cYieldingPhase;
  }

  VM* ThreadNexus::new_vm(SharedState* shared, const char* name) {
    std::lock_guard<std::mutex> guard(threads_mutex_);

    uint32_t max_id = thread_ids_;
    uint32_t id = ++thread_ids_;

    if(id < max_id) {
      rubinius::bug("exceeded maximum number of threads");
    }

    VM* vm = new VM(id, *shared, name);

    threads_.push_back(vm);

    return vm;
  }

  void ThreadNexus::delete_vm(VM* vm) {
    std::lock_guard<std::mutex> guard(threads_mutex_);

    threads_.remove(vm);
  }

  void ThreadNexus::after_fork_child(STATE) {
    new(&threads_mutex_) std::mutex;

    VM* current = state->vm();

    while(!threads_.empty()) {
      VM* vm = threads_.back()->as_vm();
      threads_.pop_back();

      if(!vm) continue;

      switch(vm->kind()) {
        case memory::ManagedThread::eThread: {
          if(Thread* thread = vm->thread()) {
            if(!thread->nil_p()) {
              if(vm == current) {
                thread->current_fiber(state, thread->fiber());
                continue;
              }

              thread->unlock_after_fork(state);
              thread->stopped();
            }
          }

          vm->reset_parked();
          vm->set_zombie();

          break;
        }
        case memory::ManagedThread::eFiber: {
          if(Fiber* fiber = vm->fiber()) {
            fiber->status(Fiber::eDead);
            vm->set_canceled();
            vm->set_zombie();
          }

          break;
        }
        case memory::ManagedThread::eSystem:
          VM::discard(state, vm);
          break;
      }
    }

    threads_.push_back(current);
    state->shared().set_root_vm(current);
  }

  static const char* phase_name(VM* vm) {
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
      if(VM* other_vm = (*i)->as_vm()) {
        function("thread %d: %s, %s",
            other_vm->thread_id(), other_vm->name().c_str(), phase_name(other_vm));
      }
    }
  }

  void ThreadNexus::detect_deadlock(STATE, uint64_t nanoseconds, VM* vm) {
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

  void ThreadNexus::wait_for_all(STATE, VM* vm) {
    std::lock_guard<std::mutex> guard(threads_mutex_);

    uint64_t limit = 0;

    set_managed(state, vm);

    for(ThreadList::iterator i = threads_.begin();
           i != threads_.end();
           ++i)
    {
      if(VM* other_vm = (*i)->as_vm()) {
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

  bool ThreadNexus::try_lock(VM* vm) {
    uint32_t id = 0;

    return lock_.compare_exchange_strong(id, vm->thread_id());
  }

  bool ThreadNexus::try_lock_wait(STATE, VM* vm) {
    uint64_t limit = 0;

    while(!try_lock(vm)) {
      if(lock_ == vm->thread_id()) {
        std::ostringstream msg;

        msg << "yielding while holding process-critical lock: id: "
            << vm->thread_id();

        Exception::raise_assertion_error(state, msg.str().c_str());
      }

      yield(state, vm);

      limit += wait();

      detect_deadlock(state, limit);
    }

    return true;
  }
}
