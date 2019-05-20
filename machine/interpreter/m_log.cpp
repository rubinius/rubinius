#include "instructions/m_log.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t m_log(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::m_log(state, call_frame, argument(0));

      call_frame->next_ip(instructions::data_m_log.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
