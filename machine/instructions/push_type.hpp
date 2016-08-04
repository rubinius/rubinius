#include "interpreter/instructions.hpp"

inline void rubinius::instruction_push_type(STATE, CallFrame* call_frame) {
  stack_push(G(type));
}
