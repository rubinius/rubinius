#include "instructions/push_type.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_type(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::push_type(state, call_frame);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
