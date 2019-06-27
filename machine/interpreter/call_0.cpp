#include "instructions/call_0.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t call_0(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t literal = argument(0);

      if(instructions::call_0(state, call_frame, literal)) {
        call_frame->next_ip(instructions::data_call_0.width
            + instructions::data_unwind.width);
      } else {
        call_frame->next_ip(instructions::data_call_0.width);
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
