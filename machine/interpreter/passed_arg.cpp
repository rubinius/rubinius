#include "instructions/passed_arg.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t passed_arg(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t index = argument(0);

      if(instructions::passed_arg(state, call_frame, index)) {
        call_frame->next_ip(instructions::data_passed_arg.width
            + instructions::data_unwind.width);
      } else {
        call_frame->next_ip(instructions::data_passed_arg.width);
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
