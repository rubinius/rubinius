#include "instructions/push_mirror.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_mirror(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::push_mirror(state, call_frame);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
