#include "instructions/push_rubinius.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_rubinius(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::push_rubinius(state, call_frame);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
