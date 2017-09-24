#include "instructions/noop.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t noop(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::noop();

      call_frame->next_ip(instructions::data_noop.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
