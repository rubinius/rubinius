#include "instructions.hpp"

#include "class/location.hpp"
#include "class/unwind_state.hpp"

namespace rubinius {
  namespace instructions {
    inline intptr_t raise_break(STATE, CallFrame* call_frame) {
      if(call_frame->flags & CallFrame::cIsLambda) {
        return (intptr_t)stack_top();
      } else if(state->scope_valid_p(call_frame->scope->parent())) {
        state->unwind_state()->raise_break(stack_top(), call_frame->scope->parent());
        return 0;
      } else {
        Exception* exc = Exception::make_exception(state, G(jump_error),
            "attempted to break to exited method");
        exc->locations(state, Location::from_call_stack(state));
        state->raise_exception(exc);
        return 0;
      }
    }
  }
}
