#include "instructions/a_type.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t a_type(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::a_type(state, call_frame, argument(0), argument(1));

      call_frame->next_ip(instructions::data_a_type.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
