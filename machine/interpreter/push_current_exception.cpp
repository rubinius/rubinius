#include "instructions/push_current_exception.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_current_exception(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::push_current_exception(state, call_frame);

      call_frame->next_ip(instructions::data_push_current_exception.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
