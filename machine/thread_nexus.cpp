#include "shared_state.hpp"
#include "thread_nexus.hpp"
#include "vm.hpp"

#include "builtin/thread.hpp"

#include "memory/managed.hpp"

#include "util/atomic.hpp"

#include "instruments/timing.hpp"

#include <time.h>

namespace rubinius {
  void ThreadNexus::blocking_phase(VM* vm) {
    bool held = waiting_lock(vm);
    vm->set_thread_phase(eBlocking);
    if(!held) unlock();
  }

  void ThreadNexus::managed_phase(VM* vm) {
    if(!waiting_lock(vm)) unlock();
  }

  void ThreadNexus::unmanaged_phase(VM* vm) {
    vm->set_thread_phase(eUnmanaged);
  }

  void ThreadNexus::waiting_phase(VM* vm) {
    vm->set_thread_phase(eWaiting);
  }

  void ThreadNexus::restore_phase(VM* vm, Phase phase) {
    switch(phase) {
    case eManaged:
      managed_phase(vm);
      break;
    case eBlocking:
      blocking_phase(vm);
      break;
    case eUnmanaged:
      unmanaged_phase(vm);
      break;
    case eWaiting:
      waiting_phase(vm);
      break;
    }
  }

  bool ThreadNexus::blocking_p(VM* vm) {
    atomic::memory_barrier();
    return (vm->thread_phase() & eBlocking) == eBlocking;
  }

  bool ThreadNexus::yielding_p(VM* vm) {
    atomic::memory_barrier();

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

    for(ThreadList::iterator i = threads_.begin();
           i != threads_.end();
           ++i) {
      if(VM* vm = (*i)->as_vm()) {
        if(vm == current) continue;

        if(vm->kind() == memory::ManagedThread::eThread) {
          if(Thread* thread = vm->thread()) {
            if(!thread->nil_p()) {
              thread->unlock_after_fork(state);
              thread->stopped();
            }
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

  void ThreadNexus::detect_deadlock(uint64_t nanoseconds, uint64_t limit, VM* vm) {
    if(nanoseconds > limit) {
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

  bool ThreadNexus::try_checkpoint(VM* vm) {
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

          detect_deadlock(ns, lock_limit, other_vm);
        }
      }
    }

    return true;
  }

  void ThreadNexus::checkpoint(VM* vm) {
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

          detect_deadlock(ns, lock_limit, other_vm);
        }
      }
    }
  }

  bool ThreadNexus::waiting_lock(VM* vm) {
    uint32_t id = 0;

    vm->set_thread_phase(eWaiting);

    while(!phase_flag_.compare_exchange_strong(id, vm->thread_id(),
          std::memory_order_acq_rel))
    {
      if(id == vm->thread_id()) {
        /* The exchange failed, but it was because the value was already set
         * to our id, so we hold the "lock".
         */
        vm->set_thread_phase(eManaged);
        return true;
      }

      {
        std::unique_lock<std::mutex> lk(wait_mutex_);
        wait_condition_.wait(lk,
            [this]{ return phase_flag_.load(std::memory_order_acquire) == 0; });
      }

      id = 0;
    }

    vm->set_thread_phase(eManaged);
    return false;
  }
}
