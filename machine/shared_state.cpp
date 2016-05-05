#include "vm.hpp"
#include "state.hpp"
#include "shared_state.hpp"
#include "call_frame.hpp"
#include "config_parser.hpp"
#include "config.h"
#include "memory.hpp"
#include "environment.hpp"
#include "instruments/timing.hpp"
#include "global_cache.hpp"

#include "util/thread.hpp"
#include "configuration.hpp"

#include "console.hpp"
#include "metrics.hpp"
#include "signal.hpp"
#include "builtin/randomizer.hpp"
#include "builtin/array.hpp"
#include "builtin/thread.hpp"
#include "builtin/native_method.hpp"

#include <iostream>
#include <iomanip>

#ifdef ENABLE_LLVM
#include "jit/llvm/state.hpp"
#endif

namespace rubinius {

  SharedState::SharedState(Environment* env, Configuration& config, ConfigParser& cp)
    : thread_nexus_(new ThreadNexus())
    , internal_threads_(NULL)
    , signals_(NULL)
    , finalizer_thread_(NULL)
    , console_(NULL)
    , metrics_(NULL)
    , diagnostics_(NULL)
    , method_count_(1)
    , class_count_(1)
    , global_serial_(1)
    , initialized_(false)
    , check_global_interrupts_(false)
    , check_gc_(false)
    , root_vm_(NULL)
    , env_(env)
    , codedb_lock_(true)
    , capi_ds_lock_()
    , capi_locks_lock_()
    , capi_constant_lock_()
    , global_capi_handle_lock_()
    , capi_handle_cache_lock_()
    , llvm_state_lock_()
    , wait_lock_()
    , type_info_lock_()
    , code_resource_lock_()
    , use_capi_lock_(false)
    , om(NULL)
    , global_cache(new GlobalCache)
    , config(config)
    , user_variables(cp)
    , llvm_state(NULL)
    , username("")
    , pid("")
  {
    internal_threads_ = new InternalThreads();

    for(int i = 0; i < Primitives::cTotalPrimitives; i++) {
      primitive_hits_[i] = 0;
    }

    hash_seed = Randomizer::random_uint32();
    initialize_capi_black_list();
  }

  SharedState::~SharedState() {
    if(!initialized_) return;

#ifdef ENABLE_LLVM
    if(llvm_state) {
      delete llvm_state;
    }
#endif

    if(console_) {
      delete console_;
      console_ = 0;
    }

    if(metrics_) {
      delete metrics_;
      metrics_ = 0;
    }

    delete global_cache;
    delete om;
    delete internal_threads_;
  }

  Array* SharedState::vm_threads(STATE) {
    utilities::thread::SpinLock::LockGuard guard(thread_nexus_->threads_lock());

    Array* threads = Array::create(state, 0);

    for(ThreadList::iterator i = thread_nexus_->threads()->begin();
        i != thread_nexus_->threads()->end();
        ++i)
    {
      if(VM* vm = (*i)->as_vm()) {
        Thread *thread = vm->thread.get();
        if(!thread->nil_p() && CBOOL(thread->alive())) {
          threads->append(state, thread);
        }
      }
    }

    return threads;
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

  metrics::Metrics* SharedState::start_metrics(STATE) {
    if(state->shared().config.system_metrics_target.value.compare("none")) {
      if(!metrics_) {
        metrics_ = new metrics::Metrics(state);
        metrics_->start(state);
        metrics_->init_ruby_metrics(state);
      }
    }

    return metrics_;
  }

  void SharedState::disable_metrics(STATE) {
    if(metrics_) metrics_->disable(state);
  }

  diagnostics::Diagnostics* SharedState::start_diagnostics(STATE) {
    if(!diagnostics_) {
      if(state->shared().config.system_diagnostics_target.value.compare("none")) {
        diagnostics_ = new diagnostics::Diagnostics(state);
        diagnostics_->start(state);
      }
    }

    return diagnostics_;
  }

  void SharedState::after_fork_child(STATE) {
    // For now, we disable inline debugging here. This makes inspecting
    // it much less confusing.
    config.jit_inline_debug.set("no");

    disable_metrics(state);

    // Reinit the locks for this object
    global_cache->reset();
    codedb_lock_.init(true);
    capi_ds_lock_.init();
    capi_locks_lock_.init();
    capi_constant_lock_.init();
    global_capi_handle_lock_.init();
    capi_handle_cache_lock_.init();
    llvm_state_lock_.init();
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
    capi_constant_name_map_[cCApiEnumerator] = "Enumerable::Enumerator";
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
