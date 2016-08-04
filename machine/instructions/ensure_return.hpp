#include "interpreter/instructions.hpp"

inline intptr_t rubinius::instruction_ensure_return(STATE, CallFrame* call_frame) {
  state->vm()->thread_state()->raise_return(stack_top(), call_frame->promote_scope(state));
  return false;
}
