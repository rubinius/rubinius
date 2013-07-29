#ifndef RBX_SHARED_STATE_H
#define RBX_SHARED_STATE_H

#include "config.h"

#include "util/refcount.hpp"
#include "gc/variable_buffer.hpp"
#include "gc/root_buffer.hpp"
#include "kcode.hpp"

#include "stats.hpp"

#include "auxiliary_threads.hpp"
#include "globals.hpp"
#include "symboltable.hpp"

#include "primitives.hpp"

#include "lock.hpp"

#include "util/thread.hpp"

#include "capi/capi_constants.h"

#include <vector>

#include "missing/unordered_map.hpp"
#include "missing/unordered_set.hpp"

#ifdef RBX_WINDOWS
#include <winsock2.h>
#endif

namespace rubinius {
  namespace capi {
    class Handle;
    class Handles;
    class GlobalHandle;
  }

  namespace tooling {
    class ToolBroker;
  }

  class SignalHandler;
  class FinalizerHandler;
  class ObjectMemory;
  class GlobalCache;
  class ConfigParser;
  class State;
  class VM;
  class Configuration;
  class LLVMState;
  class WorldState;
  class ManagedThread;
  class QueryAgent;
  class Environment;

  typedef std_unordered_set<std::string> CApiBlackList;
  typedef std::vector<Mutex*> CApiLocks;
  typedef std_unordered_map<std::string, int> CApiLockMap;

  typedef std::vector<std::string> CApiConstantNameMap;
  typedef std_unordered_map<int, capi::Handle*> CApiConstantHandleMap;

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
    AuxiliaryThreads* auxiliary_threads_;
    SignalHandler* signal_handler_;
    FinalizerHandler* finalizer_handler_;
    QueryAgent* query_agent_;

    CApiConstantNameMap capi_constant_name_map_;
    CApiConstantHandleMap capi_constant_handle_map_;

    WorldState* world_;
    std::list<ManagedThread*> threads_;

    uint64_t method_count_;
    unsigned int class_count_;
    int global_serial_;
    int thread_ids_;

    bool initialized_;
    bool ruby_critical_set_;
    bool check_global_interrupts_;
    bool check_gc_;

    kcode::CodePage kcode_page_;
    kcode::table* kcode_table_;

    QueryAgent* agent_;
    VM* root_vm_;
    Environment* env_;
    tooling::ToolBroker* tool_broker_;

    // This lock is to implement Thread.critical. It is not critical as
    // the name would make it sound.
    utilities::thread::Mutex ruby_critical_lock_;
    pthread_t ruby_critical_thread_;

    utilities::thread::SpinLock capi_ds_lock_;
    utilities::thread::SpinLock capi_locks_lock_;
    utilities::thread::SpinLock capi_constant_lock_;
    utilities::thread::SpinLock llvm_state_lock_;

    CApiBlackList capi_black_list_;
    CApiLocks capi_locks_;
    CApiLockMap capi_lock_map_;

    bool use_capi_lock_;
    int primitive_hits_[Primitives::cTotalPrimitives];

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

    int size() const;

    void set_initialized() {
      setup_capi_constant_names();
      initialized_ = true;
    }

    AuxiliaryThreads* auxiliary_threads() const {
      return auxiliary_threads_;
    }

    SignalHandler* signal_handler() const {
      return signal_handler_;
    }

    void set_signal_handler(SignalHandler* thr) {
      signal_handler_ = thr;
    }

    FinalizerHandler* finalizer_handler() const {
      return finalizer_handler_;
    }

    void set_finalizer_handler(FinalizerHandler* thr) {
      finalizer_handler_ = thr;
    }

    QueryAgent* query_agent() const {
      return query_agent_;
    }

    void set_query_agent(QueryAgent* thr) {
      query_agent_ = thr;
    }

    VM* new_vm();
    void remove_vm(VM*);

    std::list<ManagedThread*>* threads() {
      return &threads_;
    }

    Array* vm_threads(STATE);

    void add_managed_thread(ManagedThread* thr);
    void remove_managed_thread(ManagedThread* thr);

    int global_serial() const {
      return global_serial_;
    }

    int inc_global_serial(STATE) {
      return atomic::fetch_and_add(&global_serial_, (int)1);
    }

    uint32_t new_thread_id();

    int* global_serial_address() {
      return &global_serial_;
    }

    uint32_t inc_class_count(STATE) {
      return atomic::fetch_and_add(&class_count_, (uint32_t)1);
    }

    uint64_t inc_method_count(STATE) {
      return atomic::fetch_and_add(&method_count_, (uint64_t)1);
    }

    int inc_primitive_hit(int primitive) {
      return ++primitive_hits_[primitive];
    }

    int& primitive_hits(int primitive) {
      return primitive_hits_[primitive];
    }

    kcode::table* kcode_table() const {
      return kcode_table_;
    }

    kcode::CodePage kcode_page() const {
      return kcode_page_;
    }

    void set_kcode_table(kcode::table* tbl, kcode::CodePage page) {
      kcode_table_ = tbl;
      kcode_page_ = page;
    }

    QueryAgent* agent() const {
      return agent_;
    }

    QueryAgent* start_agent(STATE);

    Environment* env() const {
      return env_;
    }

    VM* root_vm() const {
      return root_vm_;
    }

    tooling::ToolBroker* tool_broker() const {
      return tool_broker_;
    }

    ObjectMemory* memory() const {
      return om;
    }

    bool check_gc_p() {
      bool c = check_gc_;
      if(unlikely(c)) {
        check_gc_ = false;
      }
      return c;
    }

    void gc_soon() {
      check_global_interrupts_ = true;
      check_gc_ = true;
    }

    bool check_global_interrupts() const {
      return check_global_interrupts_;
    }

    void set_check_global_interrupts() {
      check_global_interrupts_ = true;
    }

    void clear_check_global_interrupts() {
      check_global_interrupts_ = false;
    }

    bool* check_global_interrupts_address() {
      return &check_global_interrupts_;
    }

    const unsigned int* object_memory_mark_address() const;

    void set_use_capi_lock(bool s) {
      use_capi_lock_ = s;
    }

    utilities::thread::SpinLock& capi_ds_lock() {
      return capi_ds_lock_;
    }

    utilities::thread::SpinLock& capi_constant_lock() {
      return capi_constant_lock_;
    }

    int capi_lock_index(std::string name);

    utilities::thread::SpinLock& llvm_state_lock() {
      return llvm_state_lock_;
    }

    void scheduler_loop();

    void reinit(STATE);

    bool should_stop() const;

    bool stop_the_world(THREAD) WARN_UNUSED;
    void restart_world(THREAD);

    void stop_threads_externally();
    void restart_threads_externally();

    bool checkpoint(THREAD);
    void gc_dependent(STATE, CallFrame* call_frame);
    void gc_independent(STATE, CallFrame* call_frame);

    void gc_dependent(THREAD, utilities::thread::Condition* = NULL);
    void gc_independent(THREAD);

    void set_critical(STATE, CallFrame* call_frame);
    void clear_critical(STATE);

    void enter_capi(STATE, const char* file, int line);
    void leave_capi(STATE);

    void setup_capi_constant_names();
    CApiConstantNameMap& capi_constant_name_map() {
      return capi_constant_name_map_;
    }

    CApiConstantHandleMap& capi_constant_handle_map() {
      return capi_constant_handle_map_;
    }

    void initialize_capi_black_list();
  };
}

#endif
