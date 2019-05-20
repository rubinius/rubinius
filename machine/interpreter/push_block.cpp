#include "instructions/push_block.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_block(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::push_block(state, call_frame);

      call_frame->next_ip(instructions::data_push_block.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
