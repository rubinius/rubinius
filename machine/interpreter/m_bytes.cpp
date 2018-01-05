#include "instructions/m_bytes.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t m_bytes(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t object = argument(0);

      instructions::m_bytes(state, call_frame, object);

      call_frame->next_ip(instructions::data_m_bytes.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
