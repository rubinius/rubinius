#include "instructions/send_super_stack_with_splat.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t send_super_stack_with_splat(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t literal = argument(0);
      intptr_t count = argument(1);

      if(instructions::send_super_stack_with_splat(state, call_frame, literal, count)) {
        call_frame->next_ip(instructions::data_send_super_stack_with_splat.width);
      } else {
        call_frame->exception_ip();
      }

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
