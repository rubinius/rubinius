#include "instructions/set_call_flags.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t set_call_flags(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::set_call_flags(call_frame);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
