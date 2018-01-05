#include "instructions/p_init.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t p_init(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      const intptr_t subject = argument(0);
      const intptr_t captures = argument(1);

      instructions::p_init(state, call_frame, subject, captures);

      call_frame->next_ip(instructions::data_p_init.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
