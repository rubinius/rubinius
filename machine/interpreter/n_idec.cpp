#include "instructions/n_idec.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_idec(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_idec(call_frame, argument(0), argument(1));

      call_frame->next_ip(instructions::data_n_idec.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
