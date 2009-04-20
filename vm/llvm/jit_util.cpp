#include "llvm/jit.hpp"
#include "call_frame.hpp"
#include "builtin/object.hpp"
#include "builtin/symbol.hpp"

#include "arguments.hpp"
#include "dispatch.hpp"

using namespace rubinius;

extern "C" {
  Object* rbx_simple_send(STATE, CallFrame* call_frame, Symbol* name, int count, Object** args) {
    Object* recv = args[0];
    Arguments out_args(recv, count, args+1);
    Dispatch dis(name);

    return dis.send(state, call_frame, out_args);
  }
}
