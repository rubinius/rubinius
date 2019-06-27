#include "instructions/r_load_0.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t r_load_0(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::r_load_0(call_frame, argument(0));

      call_frame->next_ip(instructions::data_r_load_0.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
