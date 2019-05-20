#include "instructions/n_ige.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_ige(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_ige(call_frame, argument(0), argument(1), argument(2));

      call_frame->next_ip(instructions::data_n_ige.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
