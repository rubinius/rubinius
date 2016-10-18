#include "interpreter/instructions.hpp"

#include "class/location.hpp"

namespace rubinius {
  namespace instructions {
    inline void raise_return(STATE, CallFrame* call_frame) {
      if(!(call_frame->flags & CallFrame::cIsLambda) &&
         !state->vm()->scope_valid_p(call_frame->top_scope(state))) {
        Exception* exc = Exception::make_exception(state, G(jump_error), "unexpected return");
        exc->locations(state, Location::from_call_stack(state));
        state->raise_exception(exc);
      } else {
        if(call_frame->flags & CallFrame::cIsLambda) {
          state->vm()->thread_state()->raise_return(stack_top(), call_frame->promote_scope(state));
        } else {
          state->vm()->thread_state()->raise_return(stack_top(), call_frame->top_scope(state));
        }
      }
    }
  }
}
