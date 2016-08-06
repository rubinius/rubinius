#include "instructions/yield_debugger.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t yield_debugger(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::yield_debugger(state);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
