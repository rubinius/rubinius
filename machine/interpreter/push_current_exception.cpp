#include "instructions/push_current_exception.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_current_exception(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::push_current_exception(state, call_frame);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
