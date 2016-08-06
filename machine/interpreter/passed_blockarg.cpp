#include "instructions/passed_blockarg.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t passed_blockarg(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t count = argument(0);

      instructions::passed_blockarg(state, call_frame, count);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
