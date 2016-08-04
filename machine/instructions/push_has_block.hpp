#include "interpreter/instructions.hpp"

inline void rubinius::instruction_push_has_block(CallFrame* call_frame) {
  stack_push(RBOOL(CBOOL(call_frame->scope->block())));
}
