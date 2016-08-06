#include "instructions/ensure_return.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t ensure_return(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::ensure_return(state, call_frame);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
