#include "instructions/ret.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t ret(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      return instructions::ret(state, call_frame);
    }
  }
}
