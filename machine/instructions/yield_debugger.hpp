#include "interpreter/instructions.hpp"

#include "helpers.hpp"

inline void rubinius::instruction_yield_debugger(STATE) {
  Helpers::yield_debugger(state, cNil);
}
