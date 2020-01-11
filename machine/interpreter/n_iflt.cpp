#include "instructions/n_iflt.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_iflt(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_iflt(call_frame, argument(0), argument(1));

      call_frame->next_ip(instructions::data_n_iflt.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
