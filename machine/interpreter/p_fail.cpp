#include "instructions/p_fail.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t p_fail(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {

      if(const intptr_t location = instructions::p_fail(state, call_frame)) {
        call_frame->next_ip(location);
      } else {
        return reinterpret_cast<intptr_t>(cNil);
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
