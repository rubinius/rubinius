#include "instructions/a_less_equal.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t a_less_equal(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::a_less_equal(state, call_frame, argument(0), argument(1));

      call_frame->next_ip(instructions::data_a_less_equal.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
