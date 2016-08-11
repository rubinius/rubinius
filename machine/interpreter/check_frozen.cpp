#include "instructions/check_frozen.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t check_frozen(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      if(instructions::check_frozen(state, call_frame)) {
        call_frame->next_ip(instructions::data_check_frozen.width);
      } else {
        call_frame->exception_ip();
      }

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
