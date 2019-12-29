#include "instructions/r_load_m_binops.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t r_load_m_binops(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::r_load_m_binops(call_frame, argument(0), argument(1));

      call_frame->next_ip(instructions::data_r_load_m_binops.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
