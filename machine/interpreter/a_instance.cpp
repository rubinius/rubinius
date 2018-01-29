#include "instructions/a_instance.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t a_instance(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::a_instance(state, call_frame, argument(0), argument(1));

      call_frame->next_ip(instructions::data_a_instance.width);

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
