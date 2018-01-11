#include "instructions/b_if.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t b_if(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      if(instructions::b_if(call_frame, argument(0))) {
        call_frame->set_ip(argument(1));
      } else {
        call_frame->next_ip(instructions::data_b_if.width);
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
