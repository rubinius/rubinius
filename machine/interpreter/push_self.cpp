#include "instructions/push_self.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_self(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::push_self(call_frame);

      call_frame->next_ip(instructions::data_push_self.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
