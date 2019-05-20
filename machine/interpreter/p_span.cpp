#include "instructions/p_span.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t p_span(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      const intptr_t chr_set = argument(0);

      instructions::p_span(state, call_frame, chr_set);

      call_frame->next_ip(instructions::data_p_span.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
