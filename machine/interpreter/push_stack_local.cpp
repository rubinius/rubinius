#include "instructions/push_stack_local.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_stack_local(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t which = argument(0);

      instructions::push_stack_local(call_frame, which);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
