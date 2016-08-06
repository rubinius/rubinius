#include "instructions/pop_many.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t pop_many(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t count = argument(0);

      instructions::pop_many(call_frame, count);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
