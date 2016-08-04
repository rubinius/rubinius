#include "interpreter/instructions.hpp"

inline void rubinius::instruction_pop(CallFrame* call_frame) {
  (void)stack_pop();
}
