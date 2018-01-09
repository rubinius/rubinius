#include "instructions/r_load_int.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t r_load_int(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::r_load_int(call_frame, argument(0), argument(1));

      call_frame->next_ip(instructions::data_r_load_int.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
