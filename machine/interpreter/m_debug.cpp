#include "instructions/m_debug.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t m_debug(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::m_debug(call_frame, argument(0), argument(1));

      call_frame->next_ip(instructions::data_m_debug.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
