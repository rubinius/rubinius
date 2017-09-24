#include "instructions/push_memo.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_memo(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t literal = argument(0);

      instructions::push_memo(call_frame, literal);

      call_frame->next_ip(instructions::data_push_memo.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
