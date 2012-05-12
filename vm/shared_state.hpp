#ifndef RBX_SHARED_STATE_H
#define RBX_SHARED_STATE_H

#include "config.h"

#include "util/refcount.hpp"
#include "call_frame_list.hpp"
#include "gc/variable_buffer.hpp"
#include "gc/root_buffer.hpp"
#include "kcode.hpp"

#include "stats.hpp"

#include "globals.hpp"
#include "symboltable.hpp"

#include "primitives.hpp"

#include "lock.hpp"

#ifdef RBX_WINDOWS
#include <winsock2.h>
#endif

namespace rubinius {
  namespace capi {
    class Handles;
  }

  namespace tooling {
    class ToolBroker;
  }

  class SignalHandler;
  class ObjectMemory;
  class GlobalCache;
  class ConfigParser;
  class State;
  class VM;
  class Configuration;
  class LLVMState;
  class WorldState;
  class InlineCacheRegistry;
  class ManagedThread;
  class QueryAgent;
  class Environment;

  /**
   * SharedState represents the global shared state that needs to be shared
   * across all VM instances.
   *
   * Rubinius makes no use of global variables; instead, all shared state is
   * stored in a reference counted instance of this class. This makes it
   * possible in theory to have multiple independent Rubinius runtimes in a
   * single process.
   */

  class SharedState : public RefCount, public Lockable {
  private:
    bool initialized_;
    SignalHandler* signal_handler_;

    capi::Handles* global_handles_;
    capi::Handles* cached_handles_;
    std::list<capi::Handle**> global_handle_locations_;

    int global_serial_;
    WorldState* world_;
    InlineCacheRegistry* ic_registry_;
    unsigned int class_count_;
    uint64_t method_count_;
    std::list<ManagedThread*> threads_;
    int thread_ids_;

    kcode::CodePage kcode_page_;
    kcode::table* kcode_table_;

    int primitive_hits_[Primitives::cTotalPrimitives];
    QueryAgent* agent_;
    VM* root_vm_;
    Environment* env_;
    tooling::ToolBroker* tool_broker_;

    thread::Mutex onig_lock_;

    // This lock is to implement Thread.critical. It is not critical as
    // the name would make it sound.
    thread::Mutex ruby_critical_lock_;
    pthread_t ruby_critical_thread_;
    bool ruby_critical_set_;

    Mutex capi_lock_;

    bool check_gc_;

  public:
    Globals globals;
    ObjectMemory* om;
    GlobalCache* global_cache;
    Configuration& config;
    ConfigParser& user_variables;
    SymbolTable symbols;
    LLVMState* llvm_state;
    Stats stats;
    uint32_t hash_seed;

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

    capi::Handles* global_handles() {
      return global_handles_;
    }

    void add_global_handle(State*, capi::Handle* handle);
    void make_handle_cached(State*, capi::Handle* handle);

    capi::Handles* cached_handles() {
      return cached_handles_;
    }

    std::list<capi::Handle**>* global_handle_locations() {
      return &global_handle_locations_;
    }

    void add_global_handle_location(capi::Handle** loc) {
      global_handle_locations_.push_back(loc);
    }

    void del_global_handle_location(capi::Handle** loc) {
      global_handle_locations_.remove(loc);
    }

    int global_serial() {
      return global_serial_;
    }

    int inc_global_serial(STATE) {
      SYNC(state);
      return ++global_serial_;
    }

    uint32_t new_thread_id();

    int* global_serial_address() {
      return &global_serial_;
    }

    InlineCacheRegistry* ic_registry() {
      return ic_registry_;
    }

    unsigned int inc_class_count(STATE) {
      SYNC(state);
      return ++class_count_;
    }

    uint64_t inc_method_count(STATE) {
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

    QueryAgent* autostart_agent(STATE);

    void stop_agent(STATE);

    Environment* env() {
      return env_;
    }

    thread::Mutex& onig_lock() {
      return onig_lock_;
    }

    VM* root_vm() {
      return root_vm_;
    }

    tooling::ToolBroker* tool_broker() {
      return tool_broker_;
    }

    ObjectMemory* memory() {
      return om;
    }

    bool check_gc_p() {
      bool c = check_gc_;
      if (unlikely(c)) {
        check_gc_ = false;
      }
      return c;
    }

    void gc_soon() {
      check_gc_ = true;
    }

    void scheduler_loop();

    void pre_exec();
    void reinit(STATE);

    bool should_stop();

    bool stop_the_world(THREAD) WARN_UNUSED;
    void restart_world(THREAD);

    void stop_threads_externally();
    void restart_threads_externally();

    bool checkpoint(THREAD);
    void gc_dependent(STATE);
    void gc_independent(STATE);

    void gc_dependent(THREAD);
    void gc_independent(THREAD);

    void set_critical(STATE);
    void clear_critical(STATE);

    void enter_capi(STATE, const char* file, int line);
    void leave_capi(STATE);
  };
}

#endif
