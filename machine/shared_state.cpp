#include "vm.hpp"
#include "state.hpp"
#include "shared_state.hpp"
#include "call_frame.hpp"
#include "config_parser.hpp"
#include "config.h"
#include "memory.hpp"
#include "environment.hpp"

#include "util/thread.hpp"
#include "configuration.hpp"

#include "signal.hpp"

#include "class/randomizer.hpp"
#include "class/array.hpp"
#include "class/fixnum.hpp"
#include "class/thread.hpp"
#include "class/native_method.hpp"
#include "class/system.hpp"

#include "diagnostics/timing.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>

namespace rubinius {

  SharedState::SharedState(Environment* env, Machine* m, ConfigParser& cp)
    : start_time_(get_current_time())
    , initialized_(false)
    , check_global_interrupts_(false)
    , check_gc_(false)
    , root_vm_(nullptr)
    , env_(env)
    , phase_(eBooting)
    , machine_(m)
    , user_variables(cp)
  {
    for(int i = 0; i < Primitives::cTotalPrimitives; i++) {
      primitive_hits_[i] = 0;
    }

    hash_seed = Randomizer::random_uint32();
  }

  SharedState::~SharedState() {
    if(!initialized_) return;
  }

  ThreadNexus* const SharedState::thread_nexus() {
    return machine_->thread_nexus();
  }

  MachineThreads* const SharedState::machine_threads() {
    return machine_->machine_threads();
  }

  Array* SharedState::vm_threads(STATE) {
    std::lock_guard<std::mutex> guard(thread_nexus()->threads_mutex());

    Array* threads = Array::create(state, 0);

    for(ThreadList::iterator i = thread_nexus()->threads()->begin();
        i != thread_nexus()->threads()->end();
        ++i)
    {
      if(VM* vm = (*i)->as_vm()) {
        Thread *thread = vm->thread();
        if(vm->kind() == memory::ManagedThread::eThread
            &&!thread->nil_p() && CBOOL(thread->alive())) {
          threads->append(state, thread);
        }
      }
    }

    return threads;
  }

  Fixnum* SharedState::vm_threads_count(STATE) {
    std::lock_guard<std::mutex> guard(thread_nexus()->threads_mutex());

    intptr_t count = 0;

    for(ThreadList::iterator i = thread_nexus()->threads()->begin();
        i != thread_nexus()->threads()->end();
        ++i)
    {
      if(VM* vm = (*i)->as_vm()) {
        Thread *thread = vm->thread();
        if(vm->kind() == memory::ManagedThread::eThread
            &&!thread->nil_p() && CBOOL(thread->alive())) {
          count++;
        }
      }
    }

    return Fixnum::from(count);
  }

  Array* SharedState::vm_fibers(STATE) {
    std::lock_guard<std::mutex> guard(thread_nexus()->threads_mutex());

    Array* fibers = Array::create(state, 0);

    for(ThreadList::iterator i = thread_nexus()->threads()->begin();
        i != thread_nexus()->threads()->end();
        ++i)
    {
      if(VM* vm = (*i)->as_vm()) {
        if(vm->kind() == memory::ManagedThread::eFiber
            && !vm->fiber()->nil_p()
            && vm->fiber()->status() != Fiber::eDead) {
          fibers->append(state, vm->fiber());
        }
      }
    }

    return fibers;
  }

  Fixnum* SharedState::vm_fibers_count(STATE) {
    std::lock_guard<std::mutex> guard(thread_nexus()->threads_mutex());

    intptr_t count = 0;

    for(ThreadList::iterator i = thread_nexus()->threads()->begin();
        i != thread_nexus()->threads()->end();
        ++i)
    {
      if(VM* vm = (*i)->as_vm()) {
        if(vm->kind() == memory::ManagedThread::eFiber
            && !vm->fiber()->nil_p()
            && vm->fiber()->status() != Fiber::eDead) {
          count++;
        }
      }
    }

    return Fixnum::from(count);
  }

  void SharedState::vm_thread_fibers(STATE, Thread* thread,
      std::function<void (STATE, Fiber*)> f)
  {
    std::lock_guard<std::mutex> guard(thread_nexus()->threads_mutex());

    for(ThreadList::iterator i = thread_nexus()->threads()->begin();
        i != thread_nexus()->threads()->end();
        ++i)
    {
      if(VM* vm = (*i)->as_vm()) {
        if(vm->kind() == memory::ManagedThread::eFiber
            && !vm->fiber()->nil_p()
            && vm->fiber()->status() != Fiber::eDead
            && vm->fiber()->thread() == thread) {
          f(state, vm->fiber());
        }
      }
    }
  }

  Array* SharedState::vm_thread_fibers(STATE, Thread* thread) {
    Array* fibers = Array::create(state, 0);

    vm_thread_fibers(state, thread,
          [fibers](STATE, Fiber* fiber){ fibers->append(state, fiber); });

    return fibers;
  }

  double SharedState::run_time() {
    return timer::time_elapsed_seconds(start_time_);
  }

  void SharedState::after_fork_child(STATE) {
    start_time_ = get_current_time();
  }
}
