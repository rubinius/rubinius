#include "instructions/pop.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t pop(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::pop(call_frame);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
