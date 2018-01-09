#include "instructions/r_copy.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t r_copy(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::r_copy(call_frame, argument(0), argument(1));

      call_frame->next_ip(instructions::data_r_copy.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
