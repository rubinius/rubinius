#include "interpreter/instructions.hpp"

inline void rubinius::instruction_push_literal(CallFrame* call_frame, intptr_t literal) {
  stack_push(reinterpret_cast<Object*>(literal));
}
