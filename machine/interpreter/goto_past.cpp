#include "instructions/goto_past.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t goto_past(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t location = argument(0);

      instructions::goto_past(state, call_frame);
      call_frame->set_ip(location);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
