#include "instructions/push_const.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_const(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t literal = argument(0);

      instructions::push_const(state, call_frame, literal);

      call_frame->next_ip(instructions::data_push_const.width);
      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
