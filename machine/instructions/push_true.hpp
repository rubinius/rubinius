#include "interpreter/instructions.hpp"

inline void rubinius::instruction_push_true(CallFrame* call_frame) {
  stack_push(cTrue);
}
