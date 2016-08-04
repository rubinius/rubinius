#include "interpreter/instructions.hpp"

inline void rubinius::instruction_clear_exception(STATE) {
  state->vm()->thread_state()->clear_raise();
}
