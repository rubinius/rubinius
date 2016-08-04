#include "interpreter/instructions.hpp"

inline void rubinius::instruction_reraise(STATE, CallFrame* call_frame) {
  interp_assert(state->vm()->thread_state()->raise_reason() != cNone);
}
