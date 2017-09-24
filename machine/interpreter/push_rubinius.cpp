#include "instructions/push_rubinius.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_rubinius(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::push_rubinius(state, call_frame);

      call_frame->next_ip(instructions::data_push_rubinius.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
