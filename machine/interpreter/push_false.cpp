#include "instructions/push_false.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_false(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::push_false(call_frame);

      call_frame->next_ip(instructions::data_push_false.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
