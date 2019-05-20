#include "instructions/yield_splat.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t yield_splat(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t count = argument(0);

      if(instructions::yield_splat(state, call_frame, count)) {
        call_frame->next_ip(instructions::data_yield_splat.width
            + instructions::data_unwind.width);
      } else {
        call_frame->next_ip(instructions::data_yield_splat.width);
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
