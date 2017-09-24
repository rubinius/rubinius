#include "instructions/find_const.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t find_const(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t literal = argument(0);

      if(instructions::find_const(state, call_frame, literal)) {
        call_frame->next_ip(instructions::data_find_const.width
            + instructions::data_unwind.width);
      } else {
        call_frame->next_ip(instructions::data_find_const.width);
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
