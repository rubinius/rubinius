#include "instructions/n_dadd.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t n_dadd(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::n_dadd(call_frame, argument(0), argument(1), argument(2));

      call_frame->next_ip(instructions::data_n_dadd.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
