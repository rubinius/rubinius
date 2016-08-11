#include "instructions/run_exception.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t run_exception(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      switch(instructions::run_exception(state, call_frame)) {
        case instructions::cExceptionUnwind:
          stack_position(call_frame->unwind_info.stack_depth);
          call_frame->set_ip(call_frame->unwind_info.target_ip);
          break;
        case instructions::cExceptionContinue:
          call_frame->next_ip(instructions::data_run_exception.width);
          break;
        case instructions::cExceptionReturn:
          call_frame->ret_ip();
          break;
      }

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
