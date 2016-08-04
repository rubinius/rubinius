#include "interpreter/instructions.hpp"

inline void rubinius::instruction_push_undef(STATE, CallFrame* call_frame) {
  stack_push(G(undefined));
}
