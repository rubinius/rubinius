#include "instructions/r_store_ref.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t r_store_ref(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::r_store_ref(call_frame, argument(0), argument(1), argument(2));

      call_frame->next_ip(instructions::data_r_store_ref.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
