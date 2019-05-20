#include "instructions/p_choice.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t p_choice(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      const intptr_t location = argument(0);
      const intptr_t index = argument(1);

      instructions::p_choice(state, call_frame, location, index);

      call_frame->next_ip(instructions::data_p_choice.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
