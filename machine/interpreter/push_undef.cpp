#include "instructions/push_undef.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_undef(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::push_undef(state, call_frame);

      call_frame->next_ip(instructions::data_push_undef.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
