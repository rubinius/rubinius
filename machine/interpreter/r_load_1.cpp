#include "instructions/r_load_1.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t r_load_1(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::r_load_1(call_frame, argument(0));

      call_frame->next_ip(instructions::data_r_load_1.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
