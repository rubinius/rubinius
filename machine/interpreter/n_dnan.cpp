#include "instructions/n_dnan.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_dnan(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_dnan(call_frame, argument(0), argument(1));

      call_frame->next_ip(instructions::data_n_dnan.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
