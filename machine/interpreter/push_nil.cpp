#include "instructions/push_nil.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_nil(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::push_nil(call_frame);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
