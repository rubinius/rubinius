#include "instructions/r_store_local_depth.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t r_store_local_depth(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::r_store_local_depth(call_frame, argument(0), argument(1), argument(2));

      call_frame->next_ip(instructions::data_r_store_local_depth.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
