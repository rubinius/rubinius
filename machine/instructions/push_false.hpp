#include "interpreter/instructions.hpp"

inline void rubinius::instruction_push_false(CallFrame* call_frame) {
  stack_push(cFalse);
}
