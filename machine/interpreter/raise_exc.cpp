#include "instructions/raise_exc.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t raise_exc(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::raise_exc(state, call_frame);

      call_frame->exception_ip();
      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
