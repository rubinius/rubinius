#include "instructions/n_ine.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_ine(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_ine(call_frame, argument(0), argument(1), argument(2));

      call_frame->next_ip(instructions::data_n_ine.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
