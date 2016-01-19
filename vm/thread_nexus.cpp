#include "shared_state.hpp"
#include "thread_nexus.hpp"
#include "vm.hpp"

#include "builtin/thread.hpp"

#include "gc/managed.hpp"

#include "util/atomic.hpp"

#include "instruments/timing.hpp"

#include <time.h>

namespace rubinius {
  void ThreadNexus::blocking(VM* vm) {
    vm->set_thread_phase(cBlocking);
  }

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

  void ThreadNexus::list_threads() {
    for(ThreadList::iterator i = threads_.begin();
           i != threads_.end();
           ++i)
    {
      if(VM* other_vm = (*i)->as_vm()) {
        utilities::logger::fatal("thread %d: %s, %s",
            other_vm->thread_id(), other_vm->name().c_str(), phase_name(other_vm));
      }
    }
  }

#define RBX_MAX_STOP_NANOSECONDS 500000000

  void ThreadNexus::detect_lock_deadlock(uint64_t nanoseconds, VM* vm) {
    if(nanoseconds > RBX_MAX_STOP_NANOSECONDS) {
      utilities::logger::fatal("thread nexus: thread will not yield: %s, %s",
          vm->name().c_str(), phase_name(vm));

      list_threads();

      rubinius::abort();
    }
  }

  void ThreadNexus::detect_halt_deadlock(uint64_t nanoseconds, VM* vm) {
    if(nanoseconds > RBX_MAX_STOP_NANOSECONDS) {
      utilities::logger::fatal("thread nexus: thread will not yield: %s, %s",
          vm->name().c_str(), phase_name(vm));

      list_threads();

      rubinius::abort();
    }
  }

  uint64_t ThreadNexus::delay() {
    static int i = 0;
    static int delay[] = { 1, 21, 270, 482, 268, 169, 224, 481,
                           262, 79, 133, 448, 227, 249, 22 };
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
    vm->set_thread_phase(ThreadNexus::cWaiting);
    lock_.lock();
    vm->set_thread_phase(ThreadNexus::cManaged);
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
    vm->set_thread_phase(cWaiting);

    uint64_t ns = 0;

    // TODO: add thread_stop signalling
    while(true) {
      {
        utilities::thread::SpinLock::LockGuard guard(threads_lock_);

        if(threads_.size() == 1) {
          if(threads_.front()->as_vm() == vm) {
            return;
          }
        }
      }

      ns += delay();

      detect_halt_deadlock(ns, vm);
    }
  }
}
