#include "instructions/n_dmod.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_dmod(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_dmod(state, call_frame, argument(0), argument(1), argument(2));

      call_frame->next_ip(instructions::data_n_dmod.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
