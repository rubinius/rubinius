#include "instructions/a_greater_equal.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t a_greater_equal(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::a_greater_equal(state, call_frame, argument(0), argument(1));

      call_frame->next_ip(instructions::data_a_greater_equal.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
