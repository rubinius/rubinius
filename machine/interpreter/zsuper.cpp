#include "instructions/zsuper.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t zsuper(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t literal = argument(0);

      if(instructions::zsuper(state, call_frame, literal)) {
        call_frame->next_ip(instructions::data_zsuper.width);
      } else {
        call_frame->exception_ip();
      }

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
