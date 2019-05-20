#include "instructions/e_invoke_function.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t e_invoke_function(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::e_invoke_function(call_frame, argument(0), argument(1));

      call_frame->next_ip(instructions::data_e_invoke_function.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
