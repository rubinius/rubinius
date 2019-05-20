#include "instructions/m_counter.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t m_counter(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t value = argument(0);

      instructions::m_counter(state, call_frame, value);

      call_frame->next_ip(instructions::data_m_counter.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
