#include "instructions/n_dgt.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_dgt(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_dgt(call_frame, argument(0), argument(1), argument(2));

      call_frame->next_ip(instructions::data_n_dgt.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
