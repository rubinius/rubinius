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

#include "console.hpp"
#include "signal.hpp"

#include "class/randomizer.hpp"
#include "class/array.hpp"
#include "class/fixnum.hpp"
#include "class/thread.hpp"
#include "class/native_method.hpp"
#include "class/system.hpp"

#include "diagnostics/codedb.hpp"
#include "diagnostics/gc.hpp"
#include "diagnostics/machine.hpp"
#include "diagnostics/memory.hpp"
#include "diagnostics/profiler.hpp"
#include "diagnostics/timing.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>

namespace rubinius {

  SharedState::SharedState(Environment* env, Configuration& config, ConfigParser& cp)
    : thread_nexus_(new ThreadNexus())
    , machine_threads_(nullptr)
    , signals_(nullptr)
    , finalizer_(nullptr)
    , console_(nullptr)
    , jit_(nullptr)
    , diagnostics_(nullptr)
    , boot_metrics_(new diagnostics::BootMetrics())
    , codedb_metrics_(new diagnostics::CodeDBMetrics())
    , gc_metrics_(new diagnostics::GCMetrics())
    , memory_metrics_(new diagnostics::MemoryMetrics())
    , profiler_(new diagnostics::Profiler())
    , capi_constant_name_map_()
    , capi_constant_map_()
    , start_time_(get_current_time())
    , class_count_(1)
    , global_serial_(1)
    , initialized_(false)
    , check_global_interrupts_(false)
    , check_gc_(false)
    , root_vm_(nullptr)
    , env_(env)
    , codedb_lock_(true)
    , capi_ds_lock_()
    , capi_locks_lock_()
    , capi_constant_lock_()
    , wait_lock_()
    , type_info_lock_()
    , code_resource_lock_()
    , use_capi_lock_(false)
    , phase_(eBooting)
    , om(nullptr)
    , config(config)
    , user_variables(cp)
    , nodename()
    , username()
    , pid()
  {
    machine_threads_ = new MachineThreads();

    for(int i = 0; i < Primitives::cTotalPrimitives; i++) {
      primitive_hits_[i] = 0;
    }

    hash_seed = Randomizer::random_uint32();
    initialize_capi_black_list();
  }

  SharedState::~SharedState() {
    if(!initialized_) return;

    if(console_) {
      delete console_;
      console_ = nullptr;
    }

    if(jit_) {
      delete jit_;
      jit_ = nullptr;
    }

    if(diagnostics_) {
      delete diagnostics_;
      diagnostics_ = nullptr;
    }

    delete om;
    delete machine_threads_;
  }

  Array* SharedState::vm_threads(STATE) {
    std::lock_guard<std::mutex> guard(thread_nexus_->threads_mutex());

    Array* threads = Array::create(state, 0);

    for(ThreadList::iterator i = thread_nexus_->threads()->begin();
        i != thread_nexus_->threads()->end();
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
    std::lock_guard<std::mutex> guard(thread_nexus_->threads_mutex());

    native_int count = 0;

    for(ThreadList::iterator i = thread_nexus_->threads()->begin();
        i != thread_nexus_->threads()->end();
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
    std::lock_guard<std::mutex> guard(thread_nexus_->threads_mutex());

    Array* fibers = Array::create(state, 0);

    for(ThreadList::iterator i = thread_nexus_->threads()->begin();
        i != thread_nexus_->threads()->end();
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
    std::lock_guard<std::mutex> guard(thread_nexus_->threads_mutex());

    native_int count = 0;

    for(ThreadList::iterator i = thread_nexus_->threads()->begin();
        i != thread_nexus_->threads()->end();
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
    std::lock_guard<std::mutex> guard(thread_nexus_->threads_mutex());

    for(ThreadList::iterator i = thread_nexus_->threads()->begin();
        i != thread_nexus_->threads()->end();
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

  console::Console* SharedState::start_console(STATE) {
    if(!console_) {
      console_ = new console::Console(state);
      console_->start(state);
    }

    return console_;
  }

  diagnostics::Diagnostics* SharedState::start_diagnostics(STATE) {
    diagnostics_ = new diagnostics::Diagnostics(state);

    if(state->shared().config.diagnostics_target.value.compare("none")) {
      diagnostics_->start_reporter(state);

      boot_metrics_->start_reporting(state);
      codedb_metrics_->start_reporting(state);
      gc_metrics_->start_reporting(state);
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

  jit::JIT* SharedState::start_jit(STATE) {
    if(!jit_) {
      if(config.jit_enabled.value) {
        jit_ = new jit::JIT(state);
      }
    }

    return jit_;
  }

  void SharedState::after_fork_child(STATE) {
    // Reinit the locks for this object
    codedb_lock_.init(true);
    capi_ds_lock_.init();
    capi_locks_lock_.init();
    capi_constant_lock_.init();
    wait_lock_.init();
    type_info_lock_.init();
    code_resource_lock_.init();

    om->after_fork_child(state);
    signals_->after_fork_child(state);
    console_->after_fork_child(state);
  }

  const unsigned int* SharedState::object_memory_mark_address() const {
    return om->mark_address();
  }

  void SharedState::enter_capi(STATE, const char* file, int line) {
    NativeMethodEnvironment* env = state->vm()->native_method_environment;
    if(int lock_index = env->current_native_frame()->capi_lock_index()) {
      capi_locks_[lock_index - 1]->lock();
    }
  }

  void SharedState::leave_capi(STATE) {
    NativeMethodEnvironment* env = state->vm()->native_method_environment;
    if(int lock_index = env->current_native_frame()->capi_lock_index()) {
      capi_locks_[lock_index - 1]->unlock();
    }
  }

  int SharedState::capi_lock_index(std::string name) {
    utilities::thread::SpinLock::LockGuard guard(capi_locks_lock_);
    int existing = capi_lock_map_[name];
    if(existing) return existing;

    CApiBlackList::const_iterator blacklisted = capi_black_list_.find(name);

    // Only disable locks if we have capi locks disabled
    // and library is not in the blacklist.
    if(!use_capi_lock_ && blacklisted == capi_black_list_.end()) {
      capi_lock_map_[name] = 0;
      return 0;
    }

    utilities::thread::Mutex* lock = new utilities::thread::Mutex(true);
    capi_locks_.push_back(lock);

    // We use a 1 offset index, so 0 can indicate no lock used
    int lock_index = capi_locks_.size();
    capi_lock_map_[name] = lock_index;
    return lock_index;
  }

  void SharedState::setup_capi_constant_names() {
    capi_constant_name_map_.resize(cCApiMaxConstant + 1);

    capi_constant_name_map_[cCApiArray]      = "Array";
    capi_constant_name_map_[cCApiBignum]     = "Bignum";
    capi_constant_name_map_[cCApiClass]      = "Class";
    capi_constant_name_map_[cCApiComparable] = "Comparable";
    capi_constant_name_map_[cCApiData]       = "Data";
    capi_constant_name_map_[cCApiEnumerable] = "Enumerable";
    capi_constant_name_map_[cCApiFalse]      = "FalseClass";
    capi_constant_name_map_[cCApiFile]       = "File";
    capi_constant_name_map_[cCApiFixnum]     = "Fixnum";
    capi_constant_name_map_[cCApiFloat]      = "Float";
    capi_constant_name_map_[cCApiHash]       = "Hash";
    capi_constant_name_map_[cCApiInteger]    = "Integer";
    capi_constant_name_map_[cCApiIO]         = "IO";
    capi_constant_name_map_[cCApiKernel]     = "Kernel";
    capi_constant_name_map_[cCApiMatch]      = "MatchData";
    capi_constant_name_map_[cCApiModule]     = "Module";
    capi_constant_name_map_[cCApiNil]        = "NilClass";
    capi_constant_name_map_[cCApiNumeric]    = "Numeric";
    capi_constant_name_map_[cCApiObject]     = "Object";
    capi_constant_name_map_[cCApiRange]      = "Range";
    capi_constant_name_map_[cCApiRegexp]     = "Regexp";
    capi_constant_name_map_[cCApiRubinius]   = "Rubinius";
    capi_constant_name_map_[cCApiString]     = "String";
    capi_constant_name_map_[cCApiStruct]     = "Struct";
    capi_constant_name_map_[cCApiSymbol]     = "Symbol";
    capi_constant_name_map_[cCApiThread]     = "Thread";
    capi_constant_name_map_[cCApiTime]       = "Time";
    capi_constant_name_map_[cCApiTrue]       = "TrueClass";
    capi_constant_name_map_[cCApiProc]       = "Proc";
    capi_constant_name_map_[cCApiGC]         = "GC";
    capi_constant_name_map_[cCApiCAPI]       = "Rubinius::CAPI";
    capi_constant_name_map_[cCApiMethod]     = "Method";
    capi_constant_name_map_[cCApiRational]   = "Rational";
    capi_constant_name_map_[cCApiComplex]    = "Complex";
    capi_constant_name_map_[cCApiEnumerator] = "Enumerator";
    capi_constant_name_map_[cCApiMutex]      = "Mutex";
    capi_constant_name_map_[cCApiDir]        = "Dir";

    capi_constant_name_map_[cCApiArgumentError]       = "ArgumentError";
    capi_constant_name_map_[cCApiEOFError]            = "EOFError";
    capi_constant_name_map_[cCApiErrno]               = "Errno";
    capi_constant_name_map_[cCApiException]           = "Exception";
    capi_constant_name_map_[cCApiFatal]               = "FatalError";
    capi_constant_name_map_[cCApiFloatDomainError]    = "FloatDomainError";
    capi_constant_name_map_[cCApiIndexError]          = "IndexError";
    capi_constant_name_map_[cCApiInterrupt]           = "Interrupt";
    capi_constant_name_map_[cCApiIOError]             = "IOError";
    capi_constant_name_map_[cCApiLoadError]           = "LoadError";
    capi_constant_name_map_[cCApiLocalJumpError]      = "LocalJumpError";
    capi_constant_name_map_[cCApiNameError]           = "NameError";
    capi_constant_name_map_[cCApiNoMemoryError]       = "NoMemoryError";
    capi_constant_name_map_[cCApiNoMethodError]       = "NoMethodError";
    capi_constant_name_map_[cCApiNotImplementedError] = "NotImplementedError";
    capi_constant_name_map_[cCApiRangeError]          = "RangeError";
    capi_constant_name_map_[cCApiRegexpError]         = "RegexpError";
    capi_constant_name_map_[cCApiRuntimeError]        = "RuntimeError";
    capi_constant_name_map_[cCApiScriptError]         = "ScriptError";
    capi_constant_name_map_[cCApiSecurityError]       = "SecurityError";
    capi_constant_name_map_[cCApiSignalException]     = "SignalException";
    capi_constant_name_map_[cCApiStandardError]       = "StandardError";
    capi_constant_name_map_[cCApiSyntaxError]         = "SyntaxError";
    capi_constant_name_map_[cCApiSystemCallError]     = "SystemCallError";
    capi_constant_name_map_[cCApiSystemExit]          = "SystemExit";
    capi_constant_name_map_[cCApiSystemStackError]    = "SystemStackError";
    capi_constant_name_map_[cCApiTypeError]           = "TypeError";
    capi_constant_name_map_[cCApiThreadError]         = "ThreadError";
    capi_constant_name_map_[cCApiZeroDivisionError]   = "ZeroDivisionError";

    capi_constant_name_map_[cCApiMathDomainError]     = "Math::DomainError";
    capi_constant_name_map_[cCApiEncoding]            = "Encoding";
    capi_constant_name_map_[cCApiEncCompatError]      = "Encoding::CompatibilityError";
    capi_constant_name_map_[cCApiWaitReadable]        = "IO::WaitReadable";
    capi_constant_name_map_[cCApiWaitWritable]        = "IO::WaitWritable";

  }

#define CAPI_BLACK_LIST(name) capi_black_list_.insert(std::string("Init_" # name))
  void SharedState::initialize_capi_black_list() {
    CAPI_BLACK_LIST(nkf);
    CAPI_BLACK_LIST(nokogiri);
  }
}
