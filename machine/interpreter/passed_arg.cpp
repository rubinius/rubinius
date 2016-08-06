#include "instructions/passed_arg.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t passed_arg(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t index = argument(0);

      instructions::passed_arg(state, call_frame, index);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
