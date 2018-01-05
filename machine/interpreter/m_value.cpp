#include "instructions/m_value.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t m_value(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t value = argument(0);
      const intptr_t r0 = argument(1);

      instructions::m_value(state, call_frame, value, r0);

      call_frame->next_ip(instructions::data_m_value.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
