#include "instructions/check_interrupts.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t check_interrupts(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::check_interrupts(state);

      call_frame->next_ip(instructions::data_check_interrupts.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
