#include "instructions/push_int.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_int(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t number = argument(0);

      instructions::push_int(call_frame, number);

      call_frame->next_ip(instructions::data_push_int.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
