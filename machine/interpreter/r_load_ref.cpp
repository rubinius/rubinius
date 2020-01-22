#include "instructions/r_load_ref.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t r_load_ref(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::r_load_ref(call_frame, argument(0), argument(1), argument(2));

      call_frame->next_ip(instructions::data_r_load_ref.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
