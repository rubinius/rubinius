#include "instructions/p_commit.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t p_commit(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      const intptr_t location = argument(0);

      instructions::p_commit(state, call_frame);

      call_frame->next_ip(location);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
