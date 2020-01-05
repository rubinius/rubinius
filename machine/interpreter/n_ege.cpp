#include "instructions/n_ege.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_ege(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_ege(state, call_frame, argument(0), argument(1), argument(2));

      call_frame->next_ip(instructions::data_n_ege.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
