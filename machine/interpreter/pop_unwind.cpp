#include "instructions/pop_unwind.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t pop_unwind(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::pop_unwind(state, call_frame);

      call_frame->next_ip(instructions::data_pop_unwind.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
