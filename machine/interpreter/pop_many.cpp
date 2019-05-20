#include "instructions/pop_many.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t pop_many(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t count = argument(0);

      instructions::pop_many(call_frame, count);

      call_frame->next_ip(instructions::data_pop_many.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
