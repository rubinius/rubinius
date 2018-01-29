#include "instructions/a_less.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t a_less(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::a_less(state, call_frame, argument(0), argument(1));

      call_frame->next_ip(instructions::data_a_less.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
