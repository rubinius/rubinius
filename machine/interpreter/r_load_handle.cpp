#include "instructions/r_load_handle.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t r_load_handle(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::r_load_handle(state, call_frame, argument(0), argument(1));

      call_frame->next_ip(instructions::data_r_load_handle.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
