#include "instructions/n_ene.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_ene(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_ene(state, call_frame, argument(0), argument(1), argument(2));

      call_frame->next_ip(instructions::data_n_ene.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
