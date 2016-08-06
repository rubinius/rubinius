#include "instructions/string_append.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t string_append(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::string_append(state, call_frame);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
