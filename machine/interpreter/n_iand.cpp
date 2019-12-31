#include "instructions/n_iand.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_iand(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_iand(call_frame, argument(0), argument(1), argument(2));

      call_frame->next_ip(instructions::data_n_iand.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
