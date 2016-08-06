#include "instructions/instance_of.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t instance_of(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::instance_of(state, call_frame);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
