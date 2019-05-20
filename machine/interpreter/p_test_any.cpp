#include "instructions/p_test_any.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t p_test_any(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      const intptr_t n = argument(0);

      if(instructions::p_test_any(state, call_frame, n)) {
        call_frame->next_ip(instructions::data_p_test_any.width);
      } else {
        call_frame->next_ip(argument(1));
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
