#include "instructions/push_ivar.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_ivar(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t literal = argument(0);

      instructions::push_ivar(state, call_frame, literal);

      call_frame->next_ip(instructions::data_push_ivar.width);
      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
