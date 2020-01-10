#include "instructions/n_ipow_o.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_ipow_o(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_ipow_o(state, call_frame, argument(0), argument(1), argument(2));

      call_frame->next_ip(instructions::data_n_ipow_o.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
