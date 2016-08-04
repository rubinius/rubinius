#include "interpreter/instructions.hpp"

inline void rubinius::instruction_dup(CallFrame* call_frame) {
  stack_push(stack_top());
}
