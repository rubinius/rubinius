#include "instructions/set_const_at.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t set_const_at(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t literal = argument(0);

      instructions::set_const_at(state, call_frame, literal);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
