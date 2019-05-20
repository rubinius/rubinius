#include "instructions/a_receiver_method.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t a_receiver_method(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::a_receiver_method(state, call_frame, argument(0), argument(1));

      call_frame->next_ip(instructions::data_a_receiver_method.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
