#include "interpreter/instructions.hpp"

inline void rubinius::instruction_push_my_field(STATE, CallFrame* call_frame, intptr_t index) {
  stack_push(call_frame->self()->get_field(state, index));
}
