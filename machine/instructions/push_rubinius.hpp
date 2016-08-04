#include "interpreter/instructions.hpp"

inline void rubinius::instruction_push_rubinius(STATE, CallFrame* call_frame) {
  stack_push(G(rubinius));
}
