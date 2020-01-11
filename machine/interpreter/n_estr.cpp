#include "instructions/n_estr.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_estr(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_estr(state, call_frame, argument(0), argument(1), argument(2));

      call_frame->next_ip(instructions::data_n_estr.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
