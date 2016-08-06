#include "instructions/check_interrupts.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t check_interrupts(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::check_interrupts(state);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
