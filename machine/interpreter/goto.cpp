#include "instructions/goto.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t goto_(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t location = argument(0);

      // Temporary until goto_past is added
      if(location < call_frame->ip()) {
        state->vm()->checkpoint(state);
      }

      instructions::goto_(call_frame);
      call_frame->set_ip(location);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
