#include "instructions/n_dsub.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_dsub(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_dsub(call_frame, argument(0), argument(1), argument(2));

      call_frame->next_ip(instructions::data_n_dsub.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
