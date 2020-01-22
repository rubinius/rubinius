#include "instructions/r_load_ivar.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t r_load_ivar(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::r_load_ivar(state, call_frame, argument(0), argument(1), argument(2));

      call_frame->next_ip(instructions::data_r_load_ivar.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
