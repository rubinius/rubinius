#ifndef RBX_SHARED_STATE_H
#define RBX_SHARED_STATE_H

#include "util/refcount.hpp"
#include "global_lock.hpp"
#include "maps.hpp"
#include "call_frame_list.hpp"
#include "gc/variable_buffer.hpp"

#include "globals.hpp"
#include "symboltable.hpp"

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

  struct Interrupts {
    bool check;
    bool switch_task;
    bool perform_gc;
    bool check_events;
    bool reschedule;
    bool use_preempt;
    bool enable_preempt;
    bool timer;

    Interrupts() :
      check(false),
      switch_task(false),
      perform_gc(false),
      check_events(false),
      reschedule(false),
      use_preempt(false),
      enable_preempt(false),
      timer(false)
    { }
  };

  class SharedState : public RefCount {
  private:
    bool initialized_;
    GlobalLock lock_;
    SignalHandler* signal_handler_;
    CallFrameLocationList cf_locations_;
    VariableRootBuffers root_buffers_;
    capi::Handles* global_handles_;
    bool profiling_;
    profiler::ProfilerCollection* profiler_collection_;
    int global_serial_;

  public:
    Globals globals;
    ObjectMemory* om;
    GlobalCache* global_cache;
    Configuration& config;
    ConfigParser& user_variables;
    Interrupts interrupts;
    SymbolTable symbols;

  public:
    SharedState(Configuration& config, ConfigParser& cp);
    ~SharedState();

    static void discard(SharedState* ss);

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

    VariableRootBuffers* variable_buffers() {
      return &root_buffers_;
    }

    capi::Handles* global_handles() {
      return global_handles_;
    }

    bool profiling() {
      return profiling_;
    }

    int global_serial() {
      return global_serial_;
    }

    int inc_global_serial() {
      return ++global_serial_;
    }

    void enable_profiling(VM* vm);

    LookupTable* disable_profiling(VM* vm);

    void add_profiler(VM* vm, profiler::Profiler* profiler);

    void remove_profiler(VM* vm, profiler::Profiler* profiler);
  };
}

#endif
