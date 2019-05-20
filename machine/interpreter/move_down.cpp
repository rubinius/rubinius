#include "instructions/move_down.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t move_down(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t positions = argument(0);

      instructions::move_down(call_frame, positions);

      call_frame->next_ip(instructions::data_move_down.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
