#include "instructions/p_test_char_set.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t p_test_char_set(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      const intptr_t n = argument(0);

      if(instructions::p_test_char_set(state, call_frame, n)) {
        call_frame->next_ip(instructions::data_p_test_char_set.width);
      } else {
        call_frame->next_ip(argument(1));
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
