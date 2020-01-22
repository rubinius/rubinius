#include "instructions/n_dsign.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_dsign(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_dsign(call_frame, argument(0), argument(1));

      call_frame->next_ip(instructions::data_n_dsign.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
