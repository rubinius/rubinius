#include "instructions/n_epow.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_epow(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_epow(state, call_frame, argument(0), argument(1), argument(2));

      call_frame->next_ip(instructions::data_n_epow.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
