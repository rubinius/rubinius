#include "instructions/clear_exception.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t clear_exception(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::clear_exception(state);

      call_frame->next_ip(instructions::data_clear_exception.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
