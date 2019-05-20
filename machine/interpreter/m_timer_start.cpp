#include "instructions/m_timer_start.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t m_timer_start(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t timer = argument(0);

      instructions::m_timer_start(state, call_frame, timer);

      call_frame->next_ip(instructions::data_m_timer_start.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
