#include "instructions/set_const.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t set_const(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t literal = argument(0);

      instructions::set_const(state, call_frame, literal);

      call_frame->next_ip(instructions::data_set_const.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
