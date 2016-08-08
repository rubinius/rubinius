#include "instructions/push_block_arg.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_block_arg(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::push_block_arg(state, call_frame);

      call_frame->next_ip(instructions::data_push_block_arg.width);
      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
