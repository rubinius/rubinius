#include "instructions/object_to_s.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t object_to_s(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t literal = argument(0);

      instructions::object_to_s(state, call_frame, literal);

      call_frame->next_ip(instructions::data_object_to_s.width);
      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
