#include "instructions/p_char_set.hpp"
#include "instructions/p_fail.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t p_char_set(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      const intptr_t chr_set = argument(0);

      if(instructions::p_char_set(state, call_frame, chr_set)) {
        call_frame->next_ip(instructions::data_p_char_set.width);
      } else if(const intptr_t location = instructions::p_fail(state, call_frame)) {
        call_frame->next_ip(location);
      } else {
        return reinterpret_cast<intptr_t>(cNil);
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}


