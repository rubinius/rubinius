#include "instructions/p_end.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t p_end(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      return instructions::p_end(state, call_frame);
    }
  }
}
