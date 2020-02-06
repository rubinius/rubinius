#include "c_api.hpp"

namespace rubinius {
  C_API::C_API()
    : capi_constant_name_map_()
    , capi_constant_map_()
    , capi_black_list_()
    , capi_locks_()
    , capi_lock_map_()
    , use_capi_lock_(false)
    , capi_ds_lock_()
    , capi_locks_lock_()
    , capi_constant_lock_()
  {
    initialize_capi_black_list();
    setup_capi_constant_names();
  }

  void C_API::after_fork_child(STATE) {
    // Reinit the locks for this object
    new(&capi_ds_lock_) locks::spinlock_mutex;
    new(&capi_locks_lock_) locks::spinlock_mutex;
    new(&capi_constant_lock_) locks::spinlock_mutex;
  }

  void C_API::enter_capi(STATE, const char* file, int line) {
    NativeMethodEnvironment* env = state->native_method_environment;
    if(int lock_index = env->current_native_frame()->capi_lock_index()) {
      capi_locks_[lock_index - 1]->lock();
    }
  }

  void C_API::leave_capi(STATE) {
    NativeMethodEnvironment* env = state->native_method_environment;
    if(int lock_index = env->current_native_frame()->capi_lock_index()) {
      capi_locks_[lock_index - 1]->unlock();
    }
  }

  int C_API::capi_lock_index(std::string name) {
    std::lock_guard<locks::spinlock_mutex> guard(capi_locks_lock_);

    int existing = capi_lock_map_[name];
    if(existing) return existing;

    CApiBlackList::const_iterator blacklisted = capi_black_list_.find(name);

    // Only disable locks if we have capi locks disabled
    // and library is not in the blacklist.
    if(!use_capi_lock_ && blacklisted == capi_black_list_.end()) {
      capi_lock_map_[name] = 0;
      return 0;
    }

    std::mutex* lock = new std::mutex;
    capi_locks_.push_back(lock);

    // We use a 1 offset index, so 0 can indicate no lock used
    int lock_index = capi_locks_.size();
    capi_lock_map_[name] = lock_index;
    return lock_index;
  }

  void C_API::setup_capi_constant_names() {
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
  void C_API::initialize_capi_black_list() {
    CAPI_BLACK_LIST(nkf);
    CAPI_BLACK_LIST(nokogiri);
  }
}
