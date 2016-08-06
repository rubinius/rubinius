#include "instructions/push_has_block.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_has_block(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::push_has_block(call_frame);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
