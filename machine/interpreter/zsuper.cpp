#include "instructions/zsuper.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t zsuper(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t literal = argument(0);

      if(instructions::zsuper(state, call_frame, literal)) {
        call_frame->next_ip(instructions::data_zsuper.width
            + instructions::data_unwind.width);
      } else {
        call_frame->next_ip(instructions::data_zsuper.width);
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
