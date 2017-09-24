#include "instructions/push_local_depth.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_local_depth(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t depth = argument(0);
      intptr_t index = argument(1);

      if(instructions::push_local_depth(state, call_frame, depth, index)) {
        call_frame->next_ip(instructions::data_push_local_depth.width
            + instructions::data_unwind.width);
      } else {
        call_frame->next_ip(instructions::data_push_local_depth.width);
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
