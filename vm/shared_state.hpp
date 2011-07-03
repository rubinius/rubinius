#ifndef RBX_SHARED_STATE_H
#define RBX_SHARED_STATE_H

#include "util/refcount.hpp"
#include "global_lock.hpp"
#include "call_frame_list.hpp"
#include "gc/variable_buffer.hpp"
#include "gc/root_buffer.hpp"
#include "kcode.hpp"

#include "stats.hpp"

#include "globals.hpp"
#include "symboltable.hpp"

#include "primitives.hpp"

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

    Interrupts()
      : check(false)
      , perform_gc(false)
      , enable_preempt(false)
    {}

    void checked() {
      check = false;
    }

    void set_perform_gc() {
      perform_gc = true;
      check = true;
    }
  };

  class SharedState : public RefCount {
  private:
    bool initialized_;
    GlobalLock lock_;
    SignalHandler* signal_handler_;
    CallFrameLocationList cf_locations_;
    VariableRootBuffers variable_root_buffers_;
    RootBuffers         root_buffers_;

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

    GlobalLock& global_lock() {
      return lock_;
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

    CallFrameLocationList& call_frame_locations() {
      return cf_locations_;
    }

    std::list<ManagedThread*>* threads() {
      return &threads_;
    }

    void add_managed_thread(ManagedThread* thr);
    void remove_managed_thread(ManagedThread* thr);

    int new_thread_id() {
      return thread_ids_++;
    }

    VariableRootBuffers* variable_buffers() {
      return &variable_root_buffers_;
    }

    RootBuffers* root_buffers() {
      return &root_buffers_;
    }

    capi::Handles* global_handles() {
      return global_handles_;
    }

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

    int inc_global_serial() {
      return ++global_serial_;
    }

    int* global_serial_address() {
      return &global_serial_;
    }

    InlineCacheRegistry* ic_registry() {
      return ic_registry_;
    }

    unsigned int inc_class_count() {
      return ++class_count_;
    }

    uint64_t inc_method_count() {
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

    tooling::ToolBroker* tool_broker() {
      return tool_broker_;
    }

    void scheduler_loop();
    void enable_preemption();

    void pre_exec();
    void reinit();
    void stop_the_world();
    void restart_world();
    void checkpoint();
    void gc_dependent();
    void gc_independent();
  };
}

#endif
