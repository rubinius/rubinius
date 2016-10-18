#include "logger.hpp"
#include "shared_state.hpp"
#include "thread_nexus.hpp"
#include "vm.hpp"

#include "class/thread.hpp"

#include "memory/managed.hpp"

#include "util/atomic.hpp"

#include "instruments/timing.hpp"

#include <ostream>
#include <string>
#include <time.h>

namespace rubinius {
  void ThreadNexus::blocking_phase(STATE, VM* vm) {
    spinning_lock(state, vm, [vm]{ vm->set_thread_phase(eBlocking); });
  }

  void ThreadNexus::managed_phase(STATE, VM* vm) {
    spinning_lock(state, vm, [vm]{ vm->set_thread_phase(eManaged); });
  }

  void ThreadNexus::unmanaged_phase(STATE, VM* vm) {
    vm->set_thread_phase(eUnmanaged);
  }

  void ThreadNexus::waiting_phase(STATE, VM* vm) {
    vm->set_thread_phase(eWaiting);
  }

  bool ThreadNexus::blocking_p(VM* vm) {
    return (vm->thread_phase() & eBlocking) == eBlocking;
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
      case ThreadNexus::eBlocking:
        return "eBlocking";
      case ThreadNexus::eUnmanaged:
        return "eUnmanaged";
      case ThreadNexus::eWaiting:
        return "eWaiting";
    }

    return "cUnknown";
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

  void ThreadNexus::detect_deadlock(STATE, uint64_t nanoseconds, uint64_t limit, VM* vm) {
    if(nanoseconds > limit) {
      logger::error("thread nexus: thread will not yield: %s, %s",
          vm->name().c_str(), phase_name(vm));

      list_threads(logger::error);

      std::ostringstream msg;
      msg << "thread will not yield: " << vm->name().c_str() << phase_name(vm);

      Exception::raise_deadlock_error(state, msg.str().c_str());
    }
  }

  void ThreadNexus::detect_deadlock(STATE, uint64_t nanoseconds, uint64_t limit) {
    if(nanoseconds > limit) {
      logger::error("thread nexus: unable to lock, possible deadlock");

      list_threads(logger::error);

      Exception::raise_deadlock_error(state,
          "thread nexus: unable to lock, possible deadlock");
    }
  }

  uint64_t ThreadNexus::delay() {
    static int i = 0;
    static int delay[] = {
      133, 464, 254, 306, 549, 287, 358, 638, 496, 81,
      472, 288, 131, 31, 435, 258, 221, 73, 537, 854
    };
    static int modulo = sizeof(delay) / sizeof(int);
    static struct timespec ts = {0, 0};

    atomic::memory_barrier();

    uint64_t ns = ts.tv_nsec = delay[i++ % modulo];
    nanosleep(&ts, NULL);

    return ns;
  }

  ThreadNexus::LockStatus ThreadNexus::fork_lock(STATE, VM* vm) {
    waiting_phase(state, vm);

    /* Preserve the state of the phase_flag_ in situations where we have the
     * entire system serialized.
     */
    uint32_t id = 0;
    bool held = false;
    uint64_t ns = 0;

    while(!phase_flag_.compare_exchange_strong(id, vm->thread_id())) {
      if(id == vm->thread_id()) {
        /* The exchange failed, but it was because the value was already set
         * to our id, so we hold the "lock".
         */
        vm->set_thread_phase(eManaged);
        held = true;
        break;
      }

      ns += delay();

      detect_deadlock(state, ns, cLockLimit);

      id = 0;
    }

    // Checkpoint all the other threads.
    set_stop();

    ns = 0;
    while(!try_checkpoint(state, vm)) {
      ns += delay();

      detect_deadlock(state, ns, cLockLimit);
    }

    /* Lock and hold the waiting_mutex to prevent any other thread from
     * holding it across a fork() call.
     */
    ns = 0;
    while(!waiting_mutex_.try_lock()) {
      ns += delay();

      detect_deadlock(state, ns, cLockLimit);
    }

    /* Hold the logger lock so that the multi-process semaphore that the
     * logger depends on is not held across fork() calls.
     */
    ns = 0;
    while(!logger::try_lock()) {
      ns += delay();

      detect_deadlock(state, ns, cLockLimit);
    }

    return to_lock_status(held);
  }

  void ThreadNexus::fork_unlock(LockStatus status) {
    logger::unlock();

    waiting_condition_.notify_all();
    waiting_mutex_.unlock();

    if(status == eLocked) {
      phase_flag_ = 0;
    }
  }

  bool ThreadNexus::try_checkpoint(STATE, VM* vm) {
    timer::StopWatch<timer::nanoseconds> timer(
        vm->metrics().lock.stop_the_world_ns);

    std::lock_guard<std::mutex> guard(threads_mutex_);

    uint64_t ns = 0;

    for(ThreadList::iterator i = threads_.begin();
           i != threads_.end();
           ++i)
    {
      if(VM* other_vm = (*i)->as_vm()) {
        if(vm == other_vm || yielding_p(other_vm)) continue;

        while(true) {
          if(yielding_p(other_vm)) {
            break;
          } else if(blocking_p(other_vm)) {
            return false;
          }

          ns += delay();

          detect_deadlock(state, ns, cLockLimit, other_vm);
        }
      }
    }

    return true;
  }

  void ThreadNexus::checkpoint(STATE, VM* vm) {
    timer::StopWatch<timer::nanoseconds> timer(
        vm->metrics().lock.stop_the_world_ns);

    std::lock_guard<std::mutex> guard(threads_mutex_);

    uint64_t ns = 0;

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

          ns += delay();

          detect_deadlock(state, ns, cLockLimit, other_vm);
        }
      }
    }
  }

  bool ThreadNexus::waiting_lock(STATE, VM* vm) {
    uint32_t id = 0;

    vm->set_thread_phase(eWaiting);

    while(!phase_flag_.compare_exchange_strong(id, vm->thread_id())) {
      if(id == vm->thread_id()) {
        /* The exchange failed, but it was because the value was already set
         * to our id, so we hold the "lock".
         */
        vm->set_thread_phase(eManaged);
        return true;
      }

      {
        std::unique_lock<std::mutex> lk(waiting_mutex_);
        waiting_condition_.wait(lk,
            [this]{ return phase_flag_ == 0; });
      }

      id = 0;
    }

    vm->set_thread_phase(eManaged);
    return false;
  }

  void ThreadNexus::spinning_lock(STATE, VM* vm, std::function<void ()> f) {
    uint32_t id = 0;
    int spin = 0;
    bool held = false;
    uint64_t ns = 0;

    vm->set_thread_phase(eWaiting);

    while(!phase_flag_.compare_exchange_strong(id, vm->thread_id())) {
      if(id == vm->thread_id()) {
        /* The exchange failed, but it was because the value was already set
         * to our id, so we hold the "lock".
         */
        held = true;
        break;
      }

      if(++spin > cSpinLimit) {
        ns += delay();

        detect_deadlock(state, ns, cLockLimit);
      }

      id = 0;
    }

    // Call the provided function while holding the lock.
    f();

    // Release the lock unless we already held it.
    if(!held) phase_flag_ = 0;
  }
}
