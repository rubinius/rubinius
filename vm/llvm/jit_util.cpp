#include "llvm/jit.hpp"
#include "call_frame.hpp"
#include "builtin/object.hpp"
#include "builtin/symbol.hpp"
#include "builtin/system.hpp"

#include "arguments.hpp"
#include "dispatch.hpp"

using namespace rubinius;

extern "C" {
  Object* rbx_simple_send(STATE, CallFrame* call_frame, Symbol* name, int count, Object** args) {
    Object* recv = args[0];
    Arguments out_args(recv, count-1, args+1);
    Dispatch dis(name);

    return dis.send(state, call_frame, out_args);
  }

  Object* rbx_arg_error(STATE, CallFrame* call_frame, Dispatch& msg, Arguments& args,
                        int required) {
    Exception* exc =
        Exception::make_argument_error(state, required, args.total(), msg.name);
    exc->locations(state, System::vm_backtrace(state, Fixnum::from(1), call_frame));
    state->thread_state()->raise_exception(exc);

    return NULL;
  }
}
