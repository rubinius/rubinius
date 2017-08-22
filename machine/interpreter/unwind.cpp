#include "instructions/ret.hpp"
#include "instructions/unwind.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t unwind(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t ip = argument(0);

      switch(instructions::unwind(state, call_frame, ip)) {
        case instructions::cExceptionRescue:
          stack_position(call_frame->unwind_info.stack_depth);
          call_frame->set_ip(call_frame->unwind_info.target_ip);
          break;
        case instructions::cExceptionEnsure:
          stack_position(call_frame->unwind_info.stack_depth);
          call_frame->set_ip(call_frame->unwind_info.target_ip);
          break;
        case instructions::cExceptionBreak:
          call_frame->next_ip(instructions::data_unwind.width);
          break;
        case instructions::cExceptionReturn:
          return instructions::ret(state, call_frame);
        case instructions::cExceptionUnwind:
          return 0;
      }

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
