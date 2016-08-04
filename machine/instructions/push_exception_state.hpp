#include "interpreter/instructions.hpp"

#include "builtin/thread_state.hpp"

inline void rubinius::instruction_push_exception_state(STATE, CallFrame* call_frame) {
  stack_push(state->vm()->thread_state()->state_as_object(state));
}
