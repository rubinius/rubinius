#include "interpreter/instructions.hpp"

inline void rubinius::instruction_push_current_exception(STATE, CallFrame* call_frame) {
  stack_push(state->vm()->thread_state()->current_exception());
}
