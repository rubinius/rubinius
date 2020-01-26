#include "instructions/r_refcnt_inc.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t r_refcnt_inc(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::r_refcnt_inc(state, call_frame, argument(0));

      call_frame->next_ip(instructions::data_r_refcnt_inc.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
