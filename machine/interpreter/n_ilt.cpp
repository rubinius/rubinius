#include "instructions/n_ilt.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_ilt(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_ilt(call_frame, argument(0), argument(1), argument(2));

      call_frame->next_ip(instructions::data_n_ilt.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
