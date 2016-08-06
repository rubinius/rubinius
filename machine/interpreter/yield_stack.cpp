#include "instructions/yield_stack.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t yield_stack(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t count = argument(0);

      instructions::yield_stack(state, call_frame, count);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
