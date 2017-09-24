#include "instructions/setup_unwind.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t setup_unwind(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t ip = argument(0);
      intptr_t type = argument(1);

      instructions::setup_unwind(call_frame, ip, type);

      call_frame->next_ip(instructions::data_setup_unwind.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
