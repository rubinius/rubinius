#include "instructions/push_my_offset.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_my_offset(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t index = argument(0);

      instructions::push_my_offset(call_frame, index);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
