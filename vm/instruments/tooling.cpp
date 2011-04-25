#include "vm/vm.hpp"
#include "arguments.hpp"
#include "dispatch.hpp"
#include "configuration.hpp"

#include "instruments/rbxti-internal.hpp"
#include "instruments/tooling.hpp"

#include "builtin/compiledmethod.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/variable_scope.hpp"
#include "builtin/system.hpp"

namespace rubinius {
namespace tooling {

  ToolBroker::ToolBroker()
    : tool_ids_(0)
    , global_tool_data_(0)
    , results_func_(0)
    , enable_func_(0)
    , enter_method_func_(0)
    , leave_method_func_(0)
    , enter_block_func_(0)
    , leave_block_func_(0)
    , enter_gc_func_(0)
    , leave_gc_func_(0)
    , enter_script_func_(0)
    , leave_script_func_(0)
    , thread_start_func_(0)
    , thread_stop_func_(0)
    , shutdown_func_(0)
  {}

  void ToolBroker::enable(STATE) {
    if(!enable_func_) return;

    state->shared.config.jit_disabled.set("true");
    System::vm_deoptimize_all(state, Qtrue);

    enable_func_(state->tooling_env());
  }

  bool ToolBroker::available(STATE) {
    if(enable_func_) return true;
    return false;
  }

  Object* ToolBroker::results(STATE) {
    if(!results_func_) return Qnil;

    state->shared.config.jit_disabled.set("false");

    // This finds all the methods again and this time makes them available
    // for JIT.
    System::vm_deoptimize_all(state, Qfalse);

    return rbxti::s(results_func_(state->tooling_env()));
  }

  void* ToolBroker::enter_method(STATE, Dispatch& msg, Arguments& args, CompiledMethod* cm) {
    if(!enter_method_func_) return 0;

    rbxti::robject recv = rbxti::s(args.recv());
    rbxti::rsymbol name = rbxti::o(msg.name);
    rbxti::rmodule mod =  rbxti::o(msg.module);
    rbxti::rmethod meth = rbxti::o(cm);

    return enter_method_func_(state->tooling_env(), recv, name, mod, meth);
  }

  void ToolBroker::leave_method(STATE, void* tag)
  {
    if(!leave_method_func_) return;

    leave_method_func_(state->tooling_env(), tag);
  }

  void* ToolBroker::enter_block(STATE, BlockEnvironment* env, Module* i_mod)
  {
    if(!enter_block_func_) return 0;

    rbxti::rsymbol name = rbxti::o(env->top_scope()->method()->name());
    rbxti::rmodule mod =  rbxti::o(i_mod);
    rbxti::rmethod meth = rbxti::o(env->code());

    return enter_block_func_(state->tooling_env(), name, mod, meth);
  }

  void ToolBroker::leave_block(STATE, void* tag)
  {
    if(!leave_block_func_) return;
    leave_block_func_(state->tooling_env(), tag);
  }

  void* ToolBroker::enter_gc(STATE, int level)
  {
    if(!enter_gc_func_) return 0;
    return enter_gc_func_(state->tooling_env(), level);
  }

  void ToolBroker::leave_gc(STATE, void* tag)
  {
    if(!leave_gc_func_) return;
    leave_gc_func_(state->tooling_env(), tag);
  }

  void* ToolBroker::enter_script(STATE, CompiledMethod* cm)
  {
    if(!enter_script_func_) return 0;

    rbxti::rmethod meth = rbxti::o(cm);

    return enter_script_func_(state->tooling_env(), meth);
  }

  void  ToolBroker::leave_script(STATE, void* tag)
  {
    if(!leave_script_func_) return;
    leave_script_func_(state->tooling_env(), tag);
  }

  void ToolBroker::shutdown(STATE) {
    if(!shutdown_func_) return;
    shutdown_func_(state->tooling_env());
  }

  void ToolBroker::thread_start(STATE) {
    if(!thread_start_func_) return;
    thread_start_func_(state->tooling_env());
  }

  void ToolBroker::thread_stop(STATE) {
    if(!thread_stop_func_) return;
    thread_stop_func_(state->tooling_env());
  }

  void ToolBroker::set_tool_results(rbxti::results_func func) {
    results_func_ = func;
  }

  void ToolBroker::set_tool_enable(rbxti::enable_func func) {
    enable_func_ = func;
  }

  void ToolBroker::set_tool_enter_method(rbxti::enter_method func) {
    enter_method_func_ = func;
  }

  void ToolBroker::set_tool_leave_method(rbxti::leave_func func) {
    leave_method_func_ = func;
  }

  void ToolBroker::set_tool_enter_block(rbxti::enter_block func)
  {
    enter_block_func_ = func;
  }

  void ToolBroker::set_tool_leave_block(rbxti::leave_func func)
  {
    leave_block_func_ = func;
  }

  void ToolBroker::set_tool_enter_gc(rbxti::enter_gc func)
  {
    enter_gc_func_ = func;
  }

  void ToolBroker::set_tool_leave_gc(rbxti::leave_func func)
  {
    leave_gc_func_ = func;
  }

  void ToolBroker::set_tool_enter_script(rbxti::enter_script func)
  {
    enter_script_func_ = func;
  }

  void ToolBroker::set_tool_leave_script(rbxti::leave_func func)
  {
    leave_script_func_ = func;
  }

  void ToolBroker::set_tool_shutdown(rbxti::shutdown_func func) {
    shutdown_func_ = func;
  }

  void ToolBroker::set_tool_thread_start(rbxti::thread_start_func func) {
    thread_start_func_ = func;
  }

  void ToolBroker::set_tool_thread_stop(rbxti::thread_stop_func func) {
    thread_stop_func_ = func;
  }

}
}
