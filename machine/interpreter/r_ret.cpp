#include "instructions/r_ret.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t r_ret(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      state->vm()->checkpoint(state);

      return instructions::r_ret(state, call_frame, argument(0));
    }
  }
}
