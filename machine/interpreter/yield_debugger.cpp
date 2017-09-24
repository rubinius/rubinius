#include "instructions/yield_debugger.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t yield_debugger(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::yield_debugger(state);

      call_frame->next_ip(instructions::data_yield_debugger.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
