#include "instructions/push_true.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_true(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::push_true(call_frame);

      call_frame->next_ip(instructions::data_push_true.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
