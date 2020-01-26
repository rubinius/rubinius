#include "instructions/r_refcnt_dec.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t r_refcnt_dec(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::r_refcnt_dec(state, call_frame, argument(0));

      call_frame->next_ip(instructions::data_r_refcnt_dec.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
