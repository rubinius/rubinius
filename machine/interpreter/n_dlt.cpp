#include "instructions/n_dlt.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_dlt(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_dlt(call_frame, argument(0), argument(1), argument(2));

      call_frame->next_ip(instructions::data_n_dlt.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
