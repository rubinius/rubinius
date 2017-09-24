#include "instructions/push_my_field.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_my_field(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t index = argument(0);

      instructions::push_my_field(state, call_frame, index);

      call_frame->next_ip(instructions::data_push_my_field.width
          + instructions::data_unwind.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
