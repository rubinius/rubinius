#include "interpreter/instructions.hpp"

inline void rubinius::instruction_push_cpath_top(STATE, CallFrame* call_frame) {
  stack_push(G(object));
}
