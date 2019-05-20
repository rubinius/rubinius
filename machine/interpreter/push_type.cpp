#include "instructions/push_type.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_type(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::push_type(state, call_frame);

      call_frame->next_ip(instructions::data_push_type.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
