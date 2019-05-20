#include "instructions/set_call_flags.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t set_call_flags(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t flags = argument(0);

      instructions::set_call_flags(call_frame, flags);

      call_frame->next_ip(instructions::data_set_call_flags.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
