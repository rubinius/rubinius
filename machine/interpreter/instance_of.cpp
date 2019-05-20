#include "instructions/instance_of.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t instance_of(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      instructions::instance_of(state, call_frame);

      call_frame->next_ip(instructions::data_instance_of.width);
      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
