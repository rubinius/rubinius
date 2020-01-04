#include "instructions/n_promote.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_promote(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_promote(state, call_frame, argument(0), argument(1), argument(2));

      call_frame->next_ip(instructions::data_n_promote.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
