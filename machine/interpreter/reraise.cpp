#include "instructions/reraise.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t reraise(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::reraise(state, call_frame);
      call_frame->exception_ip();

      call_frame->next_ip(instructions::data_reraise.width);
      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
