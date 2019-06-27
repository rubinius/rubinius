#include "instructions/call_send.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t call_send(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t literal = argument(0);
      intptr_t count = argument(1);

      if(instructions::call_send(state, call_frame, literal, count)) {
        call_frame->next_ip(instructions::data_call_send.width
            + instructions::data_unwind.width);
      } else {
        call_frame->next_ip(instructions::data_call_send.width);
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
