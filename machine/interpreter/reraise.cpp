#include "instructions/reraise.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t reraise(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      if(instructions::reraise(state, call_frame)) {
        call_frame->next_ip(instructions::data_reraise.width
            + instructions::data_unwind.width);
      } else {
        call_frame->next_ip(instructions::data_reraise.width);
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
