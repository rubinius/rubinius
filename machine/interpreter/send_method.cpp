#include "instructions/send_method.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t send_method(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t literal = argument(0);

      instructions::send_method(state, call_frame, literal);

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
