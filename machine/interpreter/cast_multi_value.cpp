#include "instructions/cast_multi_value.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t cast_multi_value(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      if(instructions::cast_multi_value(state, call_frame)) {
        call_frame->next_ip(instructions::data_cast_multi_value.width
            + instructions::data_unwind.width);
      } else {
        call_frame->next_ip(instructions::data_cast_multi_value.width);
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
