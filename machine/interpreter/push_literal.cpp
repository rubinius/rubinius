#include "instructions/push_literal.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_literal(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t literal = argument(0);

      instructions::push_literal(call_frame, literal);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
