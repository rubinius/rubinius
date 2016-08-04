#include "interpreter/instructions.hpp"

#include "builtin/location.hpp"

inline intptr_t rubinius::instruction_raise_break(STATE, CallFrame* call_frame) {
  if(call_frame->flags & CallFrame::cIsLambda) {
    return (intptr_t)stack_top();
  } else if(state->vm()->scope_valid_p(call_frame->scope->parent())) {
    state->vm()->thread_state()->raise_break(stack_top(), call_frame->scope->parent());
    return NULL;
  } else {
    Exception* exc = Exception::make_exception(state, G(jump_error), "attempted to break to exited method");
    exc->locations(state, Location::from_call_stack(state));
    state->raise_exception(exc);
    return NULL;
  }
  // TODO: instruction exceptions
  // return false;
}
