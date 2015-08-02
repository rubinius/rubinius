#include "shared_state.hpp"
#include "thread_nexus.hpp"
#include "vm.hpp"

#include "builtin/thread.hpp"

#include "gc/managed.hpp"

#include "util/atomic.hpp"

#include <time.h>

namespace rubinius {
  bool ThreadNexus::yielding_p(VM* vm) {
    return (vm->thread_phase() & cYielding) == cYielding;
  }

  VM* ThreadNexus::new_vm(SharedState* shared, const char* name) {
    utilities::thread::SpinLock::LockGuard guard(threads_lock_);

    uint32_t max_id = thread_ids_;
    uint32_t id = atomic::fetch_and_add(&thread_ids_,
        static_cast<uint32_t>(1));

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
    wait_mutex_.init();
    wait_condition_.init();

    VM* current = state->vm();
    GCTokenImpl gct;

    for(ThreadList::iterator i = threads_.begin();
           i != threads_.end();
           ++i) {
      if(VM* vm = (*i)->as_vm()) {
        if(vm == current) continue;

        if(Thread* thread = vm->thread.get()) {
          if(!thread->nil_p()) {
            thread->unlock_after_fork(state, gct);
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

  bool ThreadNexus::locking(VM* vm) {
    int iterations = 0;
    struct timespec ts = {0, 10};

    while(true) {
      bool stopped = true;

      if(++iterations > 10000) {
        rubinius::bug("unable to halt all threads, possible deadlock");
      }

      atomic::memory_barrier();

      for(ThreadList::iterator i = threads_.begin();
             i != threads_.end();
             ++i) {
        if(VM* other_vm = (*i)->as_vm()) {
          if(vm != other_vm && !yielding_p(other_vm)) {
            stopped = false;
          }
        }
      }

      if(stopped) return true;

      nanosleep(&ts, NULL);
    }

    return false;
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
