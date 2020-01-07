#include "instructions/r_store_float.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t r_store_float(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::r_store_float(state, call_frame, argument(0), argument(1));

      call_frame->next_ip(instructions::data_r_store_float.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
