#include "instructions/dup.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t dup(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::dup(call_frame);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
