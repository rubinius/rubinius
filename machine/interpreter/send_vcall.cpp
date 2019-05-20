#include "instructions/send_vcall.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t send_vcall(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t literal = argument(0);

      if(instructions::send_vcall(state, call_frame, literal)) {
        call_frame->next_ip(instructions::data_send_vcall.width
            + instructions::data_unwind.width);
      } else {
        call_frame->next_ip(instructions::data_send_vcall.width);
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
