#include "instructions/move_down.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t move_down(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t positions = argument(0);

      instructions::move_down(call_frame, positions);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
