#include "instructions/shift_array.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t shift_array(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::shift_array(state, call_frame);

      call_frame->next_ip(instructions::data_shift_array.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
