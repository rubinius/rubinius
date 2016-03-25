#include "shared_state.hpp"
#include "thread_nexus.hpp"
#include "vm.hpp"

#include "builtin/thread.hpp"

#include "memory/managed.hpp"

#include "util/atomic.hpp"

#include "instruments/timing.hpp"

#include <time.h>

namespace rubinius {
  void ThreadNexus::blocking(VM* vm) {
    vm->set_thread_phase(cBlocking);
  }

  bool ThreadNexus::blocking_p(VM* vm) {
    atomic::memory_barrier();
    return (vm->thread_phase() & cBlocking) == cBlocking;
  }

  bool ThreadNexus::sleeping_p(VM* vm) {
    atomic::memory_barrier();
    return (vm->thread_phase() & cSleeping) == cSleeping;
  }

  bool ThreadNexus::yielding_p(VM* vm) {
    atomic::memory_barrier();
    return (vm->thread_phase() & cYielding) == cYielding;
  }

  VM* ThreadNexus::new_vm(SharedState* shared, const char* name) {
    utilities::thread::SpinLock::LockGuard guard(threads_lock_);

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
    utilities::thread::SpinLock::LockGuard guard(threads_lock_);

    threads_.remove(vm);
  }

  void ThreadNexus::after_fork_child(STATE) {
    stop_ = false;
    threads_lock_.init();
    lock_.init(true);
    sleep_lock_.init();
    wait_mutex_.init();
    wait_condition_.init();

    VM* current = state->vm();

    for(ThreadList::iterator i = threads_.begin();
           i != threads_.end();
           ++i) {
      if(VM* vm = (*i)->as_vm()) {
        if(vm == current) continue;

        if(Thread* thread = vm->thread.get()) {
          if(!thread->nil_p()) {
            thread->unlock_after_fork(state);
            thread->stopped();
          }
        }

        vm->reset_parked();
      }
    }

    threads_.clear();
    threads_.push_back(current);

    state->shared().set_root_vm(current);
  }

  static const char* phase_name(VM* vm) {
    switch(vm->thread_phase()) {
      case ThreadNexus::cStop:
        return "cStop";
      case ThreadNexus::cManaged:
        return "cManaged";
      case ThreadNexus::cUnmanaged:
        return "cUnmanaged";
      case ThreadNexus::cWaiting:
        return "cWaiting";
      case ThreadNexus::cSleeping:
        return "cSleeping";
      case ThreadNexus::cYielding:
        return "cYielding";
      case ThreadNexus::cBlocking:
        return "cBlocking";
    }

    return "cUnknown";
  }

  void ThreadNexus::list_threads() {
    for(ThreadList::iterator i = threads_.begin();
           i != threads_.end();
           ++i)
    {
      if(VM* other_vm = (*i)->as_vm()) {
        logger::fatal("thread %d: %s, %s",
            other_vm->thread_id(), other_vm->name().c_str(), phase_name(other_vm));
      }
    }
  }

#define RBX_MAX_STOP_NANOSECONDS 5000000000

  void ThreadNexus::detect_lock_deadlock(uint64_t nanoseconds, VM* vm) {
    if(nanoseconds > RBX_MAX_STOP_NANOSECONDS) {
      logger::fatal("thread nexus: thread will not yield: %s, %s",
          vm->name().c_str(), phase_name(vm));

      list_threads();

      rubinius::abort();
    }
  }

  void ThreadNexus::detect_halt_deadlock(uint64_t nanoseconds, VM* vm) {
    if(nanoseconds > RBX_MAX_STOP_NANOSECONDS) {
      logger::fatal("thread nexus: thread will not yield: %s, %s",
          vm->name().c_str(), phase_name(vm));

      list_threads();

      rubinius::abort();
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

  bool ThreadNexus::serialized_p(VM* vm) {
    timer::StopWatch<timer::nanoseconds> timer(
        vm->metrics().lock.stop_the_world_ns);

    utilities::thread::SpinLock::LockGuard guard(threads_lock_);

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

          // This thread never yielded; we could be deadlocked.
          if(vm->memory()->can_gc()) {
            detect_lock_deadlock(ns, other_vm);
          }
        }
      }
    }

    return true;
  }

  void ThreadNexus::yielding(VM* vm) {
    vm->set_thread_phase(cWaiting);

    {
      utilities::thread::Mutex::LockGuard guard(wait_mutex_);

      if(stop_) {
        wait_condition_.wait(wait_mutex_);
      }
    }

    managed_lock(vm);
  }

  void ThreadNexus::managed_lock(VM* vm) {
    vm->set_thread_phase(cWaiting);

    utilities::thread::Mutex::LockGuard guard(lock_);

    vm->set_thread_phase(cManaged);
  }

  void ThreadNexus::waiting_lock(VM* vm) {
    vm->set_thread_phase(cWaiting);
    lock_.lock();
    vm->set_thread_phase(cManaged);
  }

  void ThreadNexus::sleep_lock(VM* vm) {
    utilities::thread::Mutex::LockGuard guard(sleep_lock_);
    vm->become_managed();
  }

  void ThreadNexus::lock(VM* vm) {
    waiting_lock(vm);

    while(!serialized_p(vm)) {
      ; // wait until serialized or abort with deadlock
    }
  }

  bool ThreadNexus::stop_lock(VM* vm) {
    if(!stop_) return false;

    waiting_lock(vm);

    // Assumption about stop_ may change while we progress.
    if(stop_) {
      if(serialized_p(vm)) {
        if(stop_) {
          return true;
        }
      }
    }

    // Either we're not stop_'ing or something blocked us from serializing.
    unlock();
    return false;
  }

  void ThreadNexus::wait_till_alone(VM* vm) {
    /* We block acquiring the sleep lock so that any waking thread that raced
     * us to it will finish waking up. Any sleeping thread that wakes up after
     * we get the lock will block until the process exits because once we
     * acquire the sleep lock, we never unlock it.
     */
    sleep_lock_.lock();

    vm->set_thread_phase(cWaiting);

    uint64_t ns = 0;

    // TODO: add thread_stop signalling
    while(true) {
      bool blocking = false;

      {
        utilities::thread::SpinLock::LockGuard guard(threads_lock_);

        for(ThreadList::iterator i = threads_.begin();
               i != threads_.end();
               ++i)
        {
          if(VM* other_vm = (*i)->as_vm()) {
            if(vm == other_vm || yielding_p(other_vm)) continue;

            blocking = true;
            break;
          }
        }
      }

      if(!blocking) return;

      ns += delay();

      detect_halt_deadlock(ns, vm);
    }
  }
}
