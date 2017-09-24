#include "instructions/add_scope.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t add_scope(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::add_scope(state, call_frame);

      call_frame->next_ip(instructions::data_add_scope.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
