#include "instructions/make_array.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t make_array(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t count = argument(0);

      instructions::make_array(state, call_frame, count);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
