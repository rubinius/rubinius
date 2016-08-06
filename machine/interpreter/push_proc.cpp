#include "instructions/push_proc.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_proc(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::push_proc(state, call_frame);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
