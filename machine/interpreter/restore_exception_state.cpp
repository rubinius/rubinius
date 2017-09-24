#include "instructions/restore_exception_state.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t restore_exception_state(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::restore_exception_state(state, call_frame);

      call_frame->next_ip(instructions::data_restore_exception_state.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
