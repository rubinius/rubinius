#include "instructions/kind_of.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t kind_of(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::kind_of(state, call_frame);

      call_frame->next_ip(instructions::data_kind_of.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
