#include "instructions/n_isub.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_isub(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_isub(call_frame, argument(0), argument(1), argument(2));

      call_frame->next_ip(instructions::data_n_isub.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
