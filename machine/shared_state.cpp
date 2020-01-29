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

#include "diagnostics/codedb.hpp"
#include "diagnostics/collector.hpp"
#include "diagnostics/machine.hpp"
#include "diagnostics/memory.hpp"
#include "diagnostics/profiler.hpp"
#include "diagnostics/timing.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>

namespace rubinius {

  SharedState::SharedState(Environment* env, Machine* m, ConfigParser& cp)
    : signals_(nullptr)
    , compiler_(nullptr)
    , diagnostics_(nullptr)
    , boot_metrics_(new diagnostics::BootMetrics())
    , codedb_metrics_(new diagnostics::CodeDBMetrics())
    , collector_metrics_(new diagnostics::CollectorMetrics())
    , memory_metrics_(new diagnostics::MemoryMetrics())
    , profiler_(new diagnostics::Profiler())
    , start_time_(get_current_time())
    , class_count_(1)
    , global_serial_(1)
    , initialized_(false)
    , check_global_interrupts_(false)
    , check_gc_(false)
    , root_vm_(nullptr)
    , env_(env)
    , codedb_lock_()
    , wait_lock_()
    , type_info_lock_()
    , code_resource_lock_()
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

    if(compiler_) {
      delete compiler_;
      compiler_ = nullptr;
    }

    if(diagnostics_) {
      delete diagnostics_;
      diagnostics_ = nullptr;
    }
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

  SignalThread* SharedState::start_signals(STATE) {
    signals_ = new SignalThread(state, state->vm());
    signals_->start(state);

    return signals_;
  }

  diagnostics::Diagnostics* SharedState::start_diagnostics(STATE) {
    diagnostics_ = new diagnostics::Diagnostics(state);

    if(state->configuration()->diagnostics_target.value.compare("none")) {
      diagnostics_->start_reporter(state);

      boot_metrics_->start_reporting(state);
      codedb_metrics_->start_reporting(state);
      collector_metrics_->start_reporting(state);
      memory_metrics_->start_reporting(state);
      profiler_->start_reporting(state);
    }

    return diagnostics_;
  }

  void SharedState::report_diagnostics(diagnostics::Diagnostic* diagnostic) {
    if(diagnostics_) {
      diagnostics_->report(diagnostic);
    }
  }

  void SharedState::after_fork_child(STATE) {
    // Reinit the locks for this object
    wait_lock_.init();
    type_info_lock_.init();
    code_resource_lock_.init();

    start_time_ = get_current_time();

    signals_->after_fork_child(state);
  }
}
