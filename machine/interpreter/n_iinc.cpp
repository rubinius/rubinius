#include "instructions/n_iinc.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_iinc(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_iinc(call_frame, argument(0), argument(1));

      call_frame->next_ip(instructions::data_n_iinc.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
