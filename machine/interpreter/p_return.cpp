#include "instructions/p_return.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t p_return(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      call_frame->next_ip(instructions::p_return(state, call_frame));

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
