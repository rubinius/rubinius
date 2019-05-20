#include "instructions/a_kind.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t a_kind(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::a_kind(state, call_frame, argument(0), argument(1));

      call_frame->next_ip(instructions::data_a_kind.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
