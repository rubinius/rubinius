#include "instructions/r_store_stack.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t r_store_stack(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::r_store_stack(call_frame, argument(0));

      call_frame->next_ip(instructions::data_r_store_stack.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
