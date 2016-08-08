#include "instructions/raise_return.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t raise_return(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::raise_return(state, call_frame);
      call_frame->exception_ip();

      call_frame->next_ip(instructions::data_raise_return.width);
      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
