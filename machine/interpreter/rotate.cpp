#include "instructions/rotate.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t rotate(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t count = argument(0);

      instructions::rotate(call_frame, count);

      call_frame->next_ip(instructions::data_rotate.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
