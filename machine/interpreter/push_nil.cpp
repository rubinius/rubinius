#include "instructions/push_nil.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_nil(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::push_nil(call_frame);

      call_frame->next_ip(instructions::data_push_nil.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
