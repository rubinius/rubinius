#include "instructions/m_time_stamp.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t m_time_stamp(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t value = argument(0);
      const intptr_t flag = argument(1);

      instructions::m_time_stamp(state, call_frame, value, flag);

      call_frame->next_ip(instructions::data_m_time_stamp.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
