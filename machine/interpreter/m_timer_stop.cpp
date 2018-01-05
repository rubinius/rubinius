#include "instructions/m_timer_stop.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t m_timer_stop(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      const intptr_t ip = argument(0);
      const intptr_t flag = argument(1);

      instructions::m_timer_stop(state, call_frame, ip, flag);

      call_frame->next_ip(instructions::data_m_timer_stop.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
