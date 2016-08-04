#include "interpreter/instructions.hpp"

inline void rubinius::instruction_check_interrupts(STATE) {
  state->vm()->checkpoint(state);
}
