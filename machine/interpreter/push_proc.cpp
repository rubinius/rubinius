#include "instructions/push_proc.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t push_proc(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      if(instructions::push_proc(state, call_frame)) {
        call_frame->next_ip(instructions::data_push_proc.width
            + instructions::data_unwind.width);
      } else {
        call_frame->next_ip(instructions::data_push_proc.width);
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
