#ifndef RBX_TOOLING_HPP
#define RBX_TOOLING_HPP

#include "instruments/rbxti-internal.hpp"

namespace rubinius {
  class BlockEnvironment;
  class VM;
namespace tooling {

  const static int cTotalToolDatas = 16;

  class ToolBroker {
    int tool_ids_;
    void* global_tool_data_;

    rbxti::results_func results_func_;
    rbxti::enable_func  enable_func_;

    rbxti::enter_method enter_method_func_;
    rbxti::leave_func   leave_method_func_;

    rbxti::enter_block  enter_block_func_;
    rbxti::leave_func   leave_block_func_;

    rbxti::enter_gc     enter_gc_func_;
    rbxti::leave_func   leave_gc_func_;

    rbxti::enter_script enter_script_func_;
    rbxti::leave_func   leave_script_func_;

    rbxti::thread_start_func thread_start_func_;
    rbxti::thread_stop_func thread_stop_func_;

    rbxti::shutdown_func shutdown_func_;

  public:
    ToolBroker();

    int allocate_tool_id() {
      return tool_ids_++;
    }

    void* global_tool_data() {
      return global_tool_data_;
    }

    void set_global_tool_data(void* d) {
      global_tool_data_ = d;
    }

  public:
    void* enter_method(VM* state, Executable* exec, Module* mod, Arguments& args, CompiledMethod* cm);
    void  leave_method(VM* state, void* tag);

    void* enter_block(VM* state, BlockEnvironment* env, Module* mod);
    void  leave_block(VM* state, void* tag);

    void* enter_gc(VM* state, int level);
    void  leave_gc(VM* state, void* tag);

    void* enter_script(VM* state, CompiledMethod* cm);
    void  leave_script(VM* state, void* tag);

    void shutdown(VM* state);

    void thread_start(STATE);
    void thread_stop(STATE);

    void set_tool_enter_method(rbxti::enter_method func);
    void set_tool_leave_method(rbxti::leave_func func);

    void set_tool_enter_block(rbxti::enter_block func);
    void set_tool_leave_block(rbxti::leave_func func);

    void set_tool_enter_gc(rbxti::enter_gc func);
    void set_tool_leave_gc(rbxti::leave_func func);

    void set_tool_enter_script(rbxti::enter_script func);
    void set_tool_leave_script(rbxti::leave_func func);

    void set_tool_results(rbxti::results_func func);
    void set_tool_enable(rbxti::enable_func func);

    void set_tool_shutdown(rbxti::shutdown_func func);

    void set_tool_thread_start(rbxti::thread_start_func func);
    void set_tool_thread_stop(rbxti::thread_stop_func func);

    Object* results(STATE);
    void enable(STATE);
    bool available(STATE);
  };

  class Entry {
  protected:
    VM* state_;
    ToolBroker* broker_;
    void* tag_;

  public:
    Entry(STATE)
      : state_(state)
      , broker_(state->shared.tool_broker())
    {}
  };

  class MethodEntry : public Entry {
  public:
    MethodEntry(STATE, Executable* exec, Module* mod, Arguments& args, CompiledMethod* cm=0)
      : Entry(state)
    {
      tag_ = broker_->enter_method(state, exec, mod, args, cm);
    }

    ~MethodEntry() {
      broker_->leave_method(state_, tag_);
    }
  };

  class BlockEntry : public Entry {
  public:
    BlockEntry(STATE, BlockEnvironment* env, Module* mod)
      : Entry(state)
    {
      tag_ = broker_->enter_block(state, env, mod);
    }

    ~BlockEntry() {
      broker_->leave_block(state_, tag_);
    }
  };

  const static int GCYoung = 1;
  const static int GCMature = 2;
  const static int GCFinalizer = 4;

  class GCEntry : public Entry {
  public:
    GCEntry(STATE, int level)
      : Entry(state)
    {
      tag_ = broker_->enter_gc(state, level);
    }

    ~GCEntry() {
      broker_->leave_gc(state_, tag_);
    }
  };

  class ScriptEntry : public Entry {
  public:
    ScriptEntry(STATE, CompiledMethod* cm)
      : Entry(state)
    {
      tag_ = broker_->enter_script(state, cm);
    }

    ~ScriptEntry() {
      broker_->leave_script(state_, tag_);
    }
  };
}
}

#endif
