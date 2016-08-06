#include "instructions/pop_unwind.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t pop_unwind(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::pop_unwind(state, call_frame);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
