#include "instructions/b_if_float.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t b_if_float(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      if(instructions::b_if_float(call_frame, argument(0), argument(1))) {
        call_frame->set_ip(argument(2));
      } else {
        call_frame->next_ip(instructions::data_b_if_float.width);
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
