#include "instructions/cast_array.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t cast_array(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      if(instructions::cast_array(state, call_frame)) {
        call_frame->next_ip(instructions::data_cast_array.width
            + instructions::data_unwind.width);
      } else {
        call_frame->next_ip(instructions::data_cast_array.width);
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
