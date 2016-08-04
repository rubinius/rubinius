#include "interpreter/instructions.hpp"

inline void rubinius::instruction_push_nil(CallFrame* call_frame) {
  stack_push(cNil);
}
