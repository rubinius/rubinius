#include "instructions/push_tagged_nil.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_tagged_nil(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t tagged_nil = argument(0);

      instructions::push_tagged_nil(call_frame, tagged_nil);

      call_frame->next_ip(instructions::data_push_tagged_nil.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
