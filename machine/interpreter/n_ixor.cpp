#include "instructions/n_ixor.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_ixor(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_ixor(call_frame, argument(0), argument(1), argument(2));

      call_frame->next_ip(instructions::data_n_ixor.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
