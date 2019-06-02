#include "instructions/ret.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t ret(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      state->vm()->checkpoint(state);

      return instructions::ret(state, call_frame);
    }
  }
}
