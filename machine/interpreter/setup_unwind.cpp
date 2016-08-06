#include "instructions/setup_unwind.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t setup_unwind(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t ip = argument(0);
      intptr_t type = argument(1);

      instructions::setup_unwind(call_frame, ip, type);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
