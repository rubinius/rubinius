#include "instructions/yield_splat.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t yield_splat(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t count = argument(0);

      instructions::yield_splat(state, call_frame, count);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
