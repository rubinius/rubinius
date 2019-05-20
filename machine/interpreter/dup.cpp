#include "instructions/dup.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t dup(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::dup(call_frame);

      call_frame->next_ip(instructions::data_dup.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
