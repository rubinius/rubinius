#include "instructions/string_build.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t string_build(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t count = argument(0);

      if(instructions::string_build(state, call_frame, count)) {
        call_frame->next_ip(instructions::data_string_build.width
            + instructions::data_unwind.width);
      } else {
        call_frame->next_ip(instructions::data_string_build.width);
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
