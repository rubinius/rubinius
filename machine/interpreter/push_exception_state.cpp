#include "instructions/push_exception_state.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_exception_state(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::push_exception_state(state, call_frame);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
