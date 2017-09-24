#include "instructions/pop.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t pop(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::pop(call_frame);

      call_frame->next_ip(instructions::data_pop.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
