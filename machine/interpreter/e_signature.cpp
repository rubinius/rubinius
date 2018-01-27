#include "instructions/e_signature.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t e_signature(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::e_signature(call_frame, argument(0), argument(1));

      call_frame->next_ip(instructions::data_e_signature.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
