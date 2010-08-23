#ifndef RBX_SHARED_STATE_H
#define RBX_SHARED_STATE_H

#include "util/refcount.hpp"
#include "maps.hpp"
#include "call_frame_list.hpp"
#include "gc/variable_buffer.hpp"
#include "kcode.hpp"

#include "stats.hpp"

#include "globals.hpp"
#include "symboltable.hpp"

#include "primitives.hpp"

#include "lock.hpp"

namespace rubinius {
  namespace capi {
    class Handles;
  }

  namespace profiler {
    class Profiler;
    class ProfilerCollection;
  }

  class SignalHandler;
  class ObjectMemory;
  class GlobalCache;
  class ConfigParser;
  class VM;
  class Configuration;
  class LLVMState;
  class WorldState;
  class InlineCacheRegistry;
  class ManagedThread;
  class QueryAgent;
  class Environment;

  struct Interrupts {
    bool check;
    bool perform_gc;
    bool enable_preempt;
    bool timer;

    Interrupts() :
      check(false),
      perform_gc(false),
      enable_preempt(false),
      timer(false)
    {}

    void checked() {
      check = false;
    }

    void set_timer() {
      timer = true;
      check = true;
    }

    void set_perform_gc() {
      perform_gc = true;
      check = true;
    }
  };

  class SharedState : public RefCount, public Lockable {
  private:
    bool initialized_;
    SignalHandler* signal_handler_;
    VariableRootBuffers root_buffers_;
    capi::Handles* global_handles_;
    capi::Handles* cached_handles_;
    bool profiling_;
    profiler::ProfilerCollection* profiler_collection_;
    int global_serial_;
    WorldState* world_;
    InlineCacheRegistry* ic_registry_;
    unsigned int class_count_;
    uint64_t method_count_;
    std::list<ManagedThread*> threads_;

    kcode::CodePage kcode_page_;
    kcode::table* kcode_table_;

    bool timer_thread_started_;
    pthread_t timer_thread_;

    int primitive_hits_[Primitives::cTotalPrimitives];
    QueryAgent* agent_;
    VM* root_vm_;
    Environment* env_;

    int thread_ids_;

    thread::Mutex onig_lock_;

    // This lock is to implement Thread.critical. It is not critical as
    // the name would make it sound.
    thread::Mutex ruby_critical_lock_;
    pthread_t ruby_critical_thread_;

    Mutex capi_lock_;

  public:
    Globals globals;
    ObjectMemory* om;
    GlobalCache* global_cache;
    Configuration& config;
    ConfigParser& user_variables;
    Interrupts interrupts;
    SymbolTable symbols;
    LLVMState* llvm_state;
    Stats stats;

  public:
    SharedState(Environment* env, Configuration& config, ConfigParser& cp);
    ~SharedState();

    static void discard(SharedState* ss);

    int size();

    void set_initialized() {
      initialized_ = true;
    }

    SignalHandler* signal_handler() {
      return signal_handler_;
    }

    void set_signal_handler(SignalHandler* thr) {
      signal_handler_ = thr;
    }

    static SharedState* standalone(VM*);
    VM* new_vm();
    void remove_vm(VM*);

    std::list<ManagedThread*>* threads() {
      return &threads_;
    }

    void add_managed_thread(ManagedThread* thr);
    void remove_managed_thread(ManagedThread* thr);

    VariableRootBuffers* variable_buffers() {
      return &root_buffers_;
    }

    capi::Handles* global_handles() {
      return global_handles_;
    }

    void add_global_handle(VM*, capi::Handle* handle);
    void make_handle_cached(VM*, capi::Handle* handle);

    capi::Handles* cached_handles() {
      return cached_handles_;
    }

    bool profiling() {
      return profiling_;
    }

    bool* profiling_address() {
      return &profiling_;
    }

    int global_serial() {
      return global_serial_;
    }

    int inc_global_serial(THREAD) {
      SYNC(state);
      return ++global_serial_;
    }

    uint32_t new_thread_id(THREAD);

    int* global_serial_address() {
      return &global_serial_;
    }

    InlineCacheRegistry* ic_registry() {
      return ic_registry_;
    }

    unsigned int inc_class_count(THREAD) {
      SYNC(state);
      return ++class_count_;
    }

    uint64_t inc_method_count(THREAD) {
      SYNC(state);
      return ++method_count_;
    }

    int inc_primitive_hit(int primitive) {
      return ++primitive_hits_[primitive];
    }

    int& primitive_hits(int primitive) {
      return primitive_hits_[primitive];
    }

    kcode::table* kcode_table() {
      return kcode_table_;
    }

    kcode::CodePage kcode_page() {
      return kcode_page_;
    }

    void set_kcode_table(kcode::table* tbl, kcode::CodePage page) {
      kcode_table_ = tbl;
      kcode_page_ = page;
    }

    QueryAgent* agent() {
      return agent_;
    }

    void set_agent(QueryAgent* agent) {
      agent_ = agent;
    }

    QueryAgent* autostart_agent();

    Environment* env() {
      return env_;
    }

    thread::Mutex& onig_lock() {
      return onig_lock_;
    }

    void scheduler_loop();
    void enable_preemption();

    void enable_profiling(VM* vm);
    LookupTable* disable_profiling(VM* vm);
    void add_profiler(VM* vm, profiler::Profiler* profiler);
    void remove_profiler(VM* vm, profiler::Profiler* profiler);

    void pre_exec();
    void reinit();

    void ask_for_stopage();
    bool should_stop();

    void stop_the_world(THREAD);
    void restart_world(THREAD);

    bool checkpoint(THREAD);
    void gc_dependent(THREAD);
    void gc_independent(THREAD);

    void set_critical(STATE);
    void clear_critical(STATE);

    void enter_capi(STATE);
    void leave_capi(STATE);
  };
}

#endif
