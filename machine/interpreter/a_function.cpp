#include "instructions/a_function.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t a_function(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::a_function(state, call_frame, argument(0), argument(1));

      call_frame->next_ip(instructions::data_a_function.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
