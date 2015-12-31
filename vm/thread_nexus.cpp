#include "shared_state.hpp"
#include "thread_nexus.hpp"
#include "vm.hpp"

#include "builtin/thread.hpp"

#include "gc/managed.hpp"

#include "util/atomic.hpp"

#include "instruments/timing.hpp"

#include <time.h>

namespace rubinius {
  bool ThreadNexus::blocking_p(VM* vm) {
    return (vm->thread_phase() & cBlocking) == cBlocking;
  }

  bool ThreadNexus::yielding_p(VM* vm) {
    return (vm->thread_phase() & cYielding) == cYielding;
  }

  VM* ThreadNexus::new_vm_solo(SharedState* shared, const char* name) {
    utilities::thread::SpinLock::LockGuard guard(threads_lock_);

    uint32_t max_id = thread_ids_;
    uint32_t id = ++thread_ids_;

    if(id < max_id) {
      rubinius::bug("exceeded maximum number of threads");
    }

    return new VM(id, *shared, name);
  }

  VM* ThreadNexus::new_vm(SharedState* shared, const char* name) {
    VM* vm = new_vm_solo(shared, name);

    add_vm(vm);

    return vm;
  }

  void ThreadNexus::add_vm(VM* vm) {
    utilities::thread::SpinLock::LockGuard guard(threads_lock_);

    if(vm->tracked_p()) return;

    vm->set_tracked();
    threads_.push_back(vm);
  }

  void ThreadNexus::delete_vm(VM* vm) {
    utilities::thread::SpinLock::LockGuard guard(threads_lock_);

    threads_.remove(vm);
  }

  void ThreadNexus::after_fork_child(STATE) {
    stop_ = false;
    threads_lock_.init();
    lock_.init(true);
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
      case ThreadNexus::cYielding:
        return "cYielding";
      case ThreadNexus::cBlocking:
        return "cBlocking";
    }

    return "cUnknown";
  }

  static void abort_deadlock(ThreadList& threads, VM* vm) {
    utilities::logger::fatal("thread nexus: thread will not yield: %s, %s",
        vm->name().c_str(), phase_name(vm));

    for(ThreadList::iterator i = threads.begin();
           i != threads.end();
           ++i)
    {
      if(VM* other_vm = (*i)->as_vm()) {
        utilities::logger::fatal("thread %d: %s, %s",
            other_vm->thread_id(), other_vm->name().c_str(), phase_name(other_vm));
      }
    }

    rubinius::abort();
  }

  void ThreadNexus::blocking(VM* vm) {
    vm->set_thread_phase(cBlocking);
  }

#define RBX_MAX_STOP_ITERATIONS 10000

  bool ThreadNexus::locking(VM* vm) {
    timer::StopWatch<timer::nanoseconds> timer(
        vm->metrics().lock.stop_the_world_ns);

    for(ThreadList::iterator i = threads_.begin();
           i != threads_.end();
           ++i)
    {
      if(VM* other_vm = (*i)->as_vm()) {
        if(vm == other_vm || yielding_p(other_vm)) continue;

        while(true) {
          bool yielding = false;

          for(int j = 0; j < RBX_MAX_STOP_ITERATIONS; j++) {
            if(yielding_p(other_vm)) {
              yielding = true;
              break;
            } else if(blocking_p(other_vm)) {
              return false;
            }

            static int delay[] = { 1, 21, 270, 482, 268, 169, 224, 481,
                                   262, 79, 133, 448, 227, 249, 22 };
            static int modulo = sizeof(delay) / sizeof(int);
            static struct timespec ts = {0, 0};

            atomic::memory_barrier();

            ts.tv_nsec = delay[j % modulo];
            nanosleep(&ts, NULL);
          }

          if(yielding) break;

          // This thread never yielded; we could be deadlocked.
          if(vm->memory()->can_gc()) abort_deadlock(threads_, other_vm);
        }
      }
    }

    return true;
  }

  void ThreadNexus::yielding(VM* vm) {
    {
      utilities::thread::Mutex::LockGuard guard(wait_mutex_);

      if(stop_) {
        vm->set_thread_phase(cWaiting);
        wait_condition_.wait(wait_mutex_);
      }
    }

    {
      utilities::thread::Mutex::LockGuard guard(lock_);

      vm->set_thread_phase(cManaged);
    }
  }

  void ThreadNexus::become_managed(VM* vm) {
    if(lock_.try_lock() == utilities::thread::cLocked) {
      vm->set_thread_phase(cManaged);
      lock_.unlock();
    } else {
      yielding(vm);
    }
  }

  bool ThreadNexus::lock_or_yield(VM* vm) {
    if(lock_.try_lock() == utilities::thread::cLocked) {
      if(locking(vm)) return true;
    } else {
      yielding(vm);
    }

    return false;
  }

  bool ThreadNexus::lock_or_wait(VM* vm) {
    while(true) {
      if(lock_.try_lock() == utilities::thread::cLocked) {
        set_stop();
        if(locking(vm)) return true;
      } else {
        yielding(vm);
        set_stop();
      }
    }

    return false;
  }
}
