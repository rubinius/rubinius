#include "instructions.hpp"

#include "class/location.hpp"
#include "class/unwind_state.hpp"

namespace rubinius {
  namespace instructions {
    inline intptr_t raise_return(STATE, CallFrame* call_frame) {
      if(!(call_frame->flags & CallFrame::cIsLambda) &&
         !state->scope_valid_p(call_frame->top_scope(state))) {
        Exception* exc = Exception::make_exception(state, G(jump_error), "unexpected return");
        exc->locations(state, Location::from_call_stack(state));
        state->raise_exception(exc);
      } else {
        if(call_frame->flags & CallFrame::cIsLambda) {
          state->unwind_state()->raise_return(stack_top(), call_frame->promote_scope(state));
        } else {
          state->unwind_state()->raise_return(stack_top(), call_frame->top_scope(state));
        }
      }

      return false;
    }
  }
}
