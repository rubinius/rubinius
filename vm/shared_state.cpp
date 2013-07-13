#include "vm.hpp"
#include "shared_state.hpp"
#include "config_parser.hpp"
#include "config.h"
#include "objectmemory.hpp"
#include "environment.hpp"
#include "instruments/tooling.hpp"
#include "instruments/timing.hpp"
#include "global_cache.hpp"

#include "util/thread.hpp"
#include "configuration.hpp"

#include "agent.hpp"
#include "world_state.hpp"
#include "builtin/randomizer.hpp"
#include "builtin/array.hpp"
#include "builtin/thread.hpp"
#include "builtin/nativemethod.hpp"
#include "version.h"

#include <iostream>
#include <iomanip>

#ifdef ENABLE_LLVM
#include "llvm/state.hpp"
#endif

namespace rubinius {

  SharedState::SharedState(Environment* env, Configuration& config, ConfigParser& cp)
    : auxiliary_threads_(0)
    , signal_handler_(0)
    , finalizer_handler_(0)
    , query_agent_(0)
    , world_(new WorldState(&check_global_interrupts_))
    , method_count_(1)
    , class_count_(1)
    , global_serial_(1)
    , thread_ids_(1)
    , initialized_(false)
    , ruby_critical_set_(false)
    , check_global_interrupts_(false)
    , check_gc_(false)
    , kcode_page_(kcode::eAscii)
    , kcode_table_(kcode::null_table())
    , agent_(0)
    , root_vm_(0)
    , env_(env)
    , tool_broker_(new tooling::ToolBroker)
    , use_capi_lock_(false)
    , om(0)
    , global_cache(new GlobalCache)
    , config(config)
    , user_variables(cp)
    , llvm_state(0)
  {
    ref();

    auxiliary_threads_ = new AuxiliaryThreads();

    for(int i = 0; i < Primitives::cTotalPrimitives; i++) {
      primitive_hits_[i] = 0;
    }

    hash_seed = Randomizer::random_uint32();
    initialize_capi_black_list();
  }

  SharedState::~SharedState() {
    if(!initialized_) return;

    if(config.gc_show) {
      std::cerr << "Time spent waiting for the world to stop: " << world_->time_waiting() << "ns\n";
    }

#ifdef ENABLE_LLVM
    if(llvm_state) {
      delete llvm_state;
    }
#endif

    if(agent_) {
      delete agent_;
    }

    delete tool_broker_;
    delete global_cache;
    delete world_;
    delete om;
    delete auxiliary_threads_;
  }

  void SharedState::add_managed_thread(ManagedThread* thr) {
    SYNC_TL;
    threads_.push_back(thr);
  }

  void SharedState::remove_managed_thread(ManagedThread* thr) {
    SYNC_TL;
    threads_.remove(thr);
  }

  int SharedState::size() const {
    return sizeof(SharedState) +
      sizeof(WorldState) +
      symbols.byte_size();
  }

  void SharedState::discard(SharedState* ss) {
    if(ss->deref()) delete ss;
  }

  uint32_t SharedState::new_thread_id() {
    return atomic::fetch_and_add(&thread_ids_, 1);
  }

  VM* SharedState::new_vm() {
    uint32_t id = new_thread_id();

    SYNC_TL;

    // TODO calculate the thread id by finding holes in the
    // field of ids, so we reuse ids.

    VM* vm = new VM(id, *this);
    threads_.push_back(vm);

    this->ref();

    // If there is no root vm, then the first one created becomes it.
    if(!root_vm_) root_vm_ = vm;
    return vm;
  }

  void SharedState::remove_vm(VM* vm) {
    SYNC_TL;
    threads_.remove(vm);
    this->deref();

    // Don't delete ourself here, it's too problematic.
  }

  Array* SharedState::vm_threads(STATE) {
    SYNC_TL;

    Array* threads = Array::create(state, 0);
    for(std::list<ManagedThread*>::iterator i = threads_.begin();
        i != threads_.end();
        ++i) {
      if(VM* vm = (*i)->as_vm()) {
        Thread *thread = vm->thread.get();
        if(!thread->system_thread() && CBOOL(thread->alive())) {
          threads->append(state, thread);
        }
      }
    }
    return threads;
  }

  QueryAgent* SharedState::start_agent(STATE) {
    SYNC(state);

    if(!agent_) {
      agent_ = new QueryAgent(state);
    }

    return agent_;
  }

  void SharedState::reinit(STATE) {
    // For now, we disable inline debugging here. This makes inspecting
    // it much less confusing.

    config.jit_inline_debug.set("no");

    env_->set_root_vm(state->vm());
    threads_.clear();
    threads_.push_back(state->vm());

    // Reinit the locks for this object
    lock_init(state->vm());
    global_cache->reset();
    ruby_critical_lock_.init();
    capi_ds_lock_.init();
    capi_locks_lock_.init();
    capi_constant_lock_.init();
    auxiliary_threads_->init();

    world_->reinit();
  }

  bool SharedState::should_stop() const {
    return world_->should_stop();
  }

  bool SharedState::stop_the_world(THREAD) {
    return world_->wait_til_alone(state);
  }

  void SharedState::stop_threads_externally() {
    world_->stop_threads_externally();
  }

  void SharedState::restart_world(THREAD) {
    world_->wake_all_waiters(state);
  }

  void SharedState::restart_threads_externally() {
    world_->restart_threads_externally();
  }

  bool SharedState::checkpoint(THREAD) {
    return world_->checkpoint(state);
  }

  void SharedState::gc_dependent(STATE, CallFrame* call_frame) {
    state->set_call_frame(call_frame);
    world_->become_dependent(state->vm());
  }

  void SharedState::gc_independent(STATE, CallFrame* call_frame) {
    state->set_call_frame(call_frame);
    world_->become_independent(state->vm());
  }

  void SharedState::gc_dependent(THREAD, utilities::thread::Condition* cond) {
    world_->become_dependent(state, cond);
  }

  void SharedState::gc_independent(THREAD) {
    world_->become_independent(state);
  }

  const unsigned int* SharedState::object_memory_mark_address() {
    return om->mark_address();
  }

  void SharedState::set_critical(STATE, CallFrame* call_frame) {
    SYNC(state);

    if(!ruby_critical_set_ ||
         !pthread_equal(ruby_critical_thread_, pthread_self())) {

      UNSYNC;
      GCIndependent gc_guard(state, call_frame);
      ruby_critical_lock_.lock();
      ruby_critical_thread_ = pthread_self();
      ruby_critical_set_ = true;
    }

    return;
  }

  void SharedState::clear_critical(STATE) {
    SYNC(state);

    if(ruby_critical_set_ && pthread_equal(ruby_critical_thread_, pthread_self())) {
      ruby_critical_set_ = false;
      ruby_critical_lock_.unlock();
    }
  }

  void SharedState::enter_capi(STATE, const char* file, int line) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    if(int lock_index = env->current_native_frame()->capi_lock_index()) {
      capi_locks_[lock_index - 1]->lock(state->vm(), file, line);
    }
  }

  void SharedState::leave_capi(STATE) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    if(int lock_index = env->current_native_frame()->capi_lock_index()) {
      capi_locks_[lock_index - 1]->unlock(state->vm());
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

    Mutex* lock = new Mutex();
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

    if(!LANGUAGE_18_ENABLED) {
      capi_constant_name_map_[cCApiMathDomainError]     = "Math::DomainError";
      capi_constant_name_map_[cCApiEncoding]            = "Encoding";
      capi_constant_name_map_[cCApiEncCompatError]      = "Encoding::CompatibilityError";
      capi_constant_name_map_[cCApiWaitReadable]        = "IO::WaitReadable";
      capi_constant_name_map_[cCApiWaitWritable]        = "IO::WaitWritable";
    }

  }

#define CAPI_BLACK_LIST(name) capi_black_list_.insert(std::string("Init_" # name))
  void SharedState::initialize_capi_black_list() {
    CAPI_BLACK_LIST(nkf);
  }
}
