#include "instructions/push_false.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_false(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::push_false(call_frame);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
