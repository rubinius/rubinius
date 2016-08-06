#include "instructions/push_undef.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_undef(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::push_undef(state, call_frame);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
