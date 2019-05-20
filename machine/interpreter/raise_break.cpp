#include "instructions/raise_break.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t raise_break(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      if(intptr_t value = instructions::raise_break(state, call_frame)) {
        return value;
      } else {
        call_frame->next_ip(instructions::data_raise_break.width);
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
