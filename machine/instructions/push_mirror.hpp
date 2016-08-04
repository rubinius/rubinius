#include "interpreter/instructions.hpp"

inline void rubinius::instruction_push_mirror(STATE, CallFrame* call_frame) {
  stack_push(G(mirror));
}
