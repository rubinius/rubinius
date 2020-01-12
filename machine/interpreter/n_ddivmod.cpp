#include "instructions/n_ddivmod.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_ddivmod(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_ddivmod(state, call_frame, argument(0), argument(1), argument(2));

      call_frame->next_ip(instructions::data_n_ddivmod.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
