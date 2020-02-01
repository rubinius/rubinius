#include "instructions/ret.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t ret(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t value = instructions::ret(state, call_frame);

      state->checkpoint(state);

      return value;
    }
  }
}
