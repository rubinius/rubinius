#include "instructions/push_variables.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_variables(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::push_variables(state, call_frame);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
