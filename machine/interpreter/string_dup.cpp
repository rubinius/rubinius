#include "instructions/string_dup.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t string_dup(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      if(instructions::string_dup(state, call_frame)) {
        call_frame->next_ip(instructions::data_string_dup.width);
      } else {
        call_frame->exception_ip();
      }

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
