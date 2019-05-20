#include "instructions/raise_exc.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t raise_exc(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      if(instructions::raise_exc(state, call_frame)) {
        call_frame->next_ip(instructions::data_raise_exc.width
            + instructions::data_unwind.width);
      } else {
        call_frame->next_ip(instructions::data_raise_exc.width);
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
