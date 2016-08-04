#include "interpreter/instructions.hpp"

inline void rubinius::instruction_set_stack_local(CallFrame* call_frame, intptr_t which) {
  stack_local(which) = stack_top();
}
