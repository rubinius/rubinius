#include "interpreter/instructions.hpp"

inline void rubinius::instruction_push_stack_local(CallFrame* call_frame, intptr_t which) {
  stack_push(stack_local(which));
}
