#include "instructions/e_cache_value.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t e_cache_value(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::e_cache_value(call_frame, argument(0), argument(1));

      call_frame->next_ip(instructions::data_e_cache_value.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
