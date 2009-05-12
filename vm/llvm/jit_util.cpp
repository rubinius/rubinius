#include "llvm/jit.hpp"
#include "call_frame.hpp"
#include "builtin/object.hpp"
#include "builtin/symbol.hpp"
#include "builtin/system.hpp"
#include "builtin/class.hpp"
#include "builtin/string.hpp"
#include "builtin/block_environment.hpp"

#include "arguments.hpp"
#include "dispatch.hpp"
#include "lookup_data.hpp"

using namespace rubinius;

extern "C" {
  Object* rbx_simple_send(STATE, CallFrame* call_frame, Symbol* name,
                          int count, Object** args) {
    Object* recv = args[0];
    Arguments out_args(recv, count, args+1);
    Dispatch dis(name);

    return dis.send(state, call_frame, out_args);
  }

  Object* rbx_simple_send_private(STATE, CallFrame* call_frame, Symbol* name,
                                  int count, Object** args) {
    Object* recv = args[0];
    Arguments out_args(recv, count, args+1);
    LookupData lookup(recv, recv->lookup_begin(state), true);
    Dispatch dis(name);

    return dis.send(state, call_frame, lookup, out_args);
  }

  Object* rbx_block_send(STATE, CallFrame* call_frame, Symbol* name,
                          int count, Object** args) {
    Object* recv = args[0];
    Arguments out_args(recv, args[count+1], count, args+1);
    Dispatch dis(name);

    return dis.send(state, call_frame, out_args);
  }

  Object* rbx_block_send_private(STATE, CallFrame* call_frame, Symbol* name,
                                  int count, Object** args) {
    Object* recv = args[0];
    Arguments out_args(recv, args[count+1], count, args+1);
    LookupData lookup(recv, recv->lookup_begin(state), true);
    Dispatch dis(name);

    return dis.send(state, call_frame, lookup, out_args);
  }

  Object* rbx_arg_error(STATE, CallFrame* call_frame, Dispatch& msg, Arguments& args,
                        int required) {
    Exception* exc =
        Exception::make_argument_error(state, required, args.total(), msg.name);
    exc->locations(state, System::vm_backtrace(state, Fixnum::from(1), call_frame));
    state->thread_state()->raise_exception(exc);

    return NULL;
  }

  Object* rbx_string_dup(STATE, CallFrame* call_frame, Object* obj) {
    try {
      return as<String>(obj)->string_dup(state);
    } catch(TypeError& e) {
      Exception* exc =
        Exception::make_type_error(state, e.type, e.object, e.reason);
      exc->locations(state, System::vm_backtrace(state, 0, call_frame));

      state->thread_state()->raise_exception(exc);
      return NULL;
    }
  }

  Object* rbx_create_block(STATE, CallFrame* call_frame, int index) {
    Object* _lit = call_frame->cm->literals()->at(state, index);
    CompiledMethod* cm = as<CompiledMethod>(_lit);

    call_frame->promote_scope(state);

    // TODO: We don't need to be doing this everytime.
    cm->scope(state, call_frame->static_scope);

    return BlockEnvironment::under_call_frame(state, cm, cm->backend_method_,
                                              call_frame, index);
  }
}
