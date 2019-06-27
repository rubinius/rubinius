#include "instructions/goto_future.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t goto_future(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t location = argument(0);

      instructions::goto_future(call_frame);
      call_frame->set_ip(location);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
