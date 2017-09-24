#include "instructions/set_local.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t set_local(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t local = argument(0);

      instructions::set_local(call_frame, local);

      call_frame->next_ip(instructions::data_set_local.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
