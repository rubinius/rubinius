#include "instructions/ensure_return.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t ensure_return(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      if(instructions::ensure_return(state, call_frame)) {
        call_frame->next_ip(instructions::data_ensure_return.width
            + instructions::data_unwind.width);
      } else {
        call_frame->next_ip(instructions::data_ensure_return.width);
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
