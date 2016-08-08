#include "instructions/run_exception.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t run_exception(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      if(instructions::run_exception(state, call_frame)) {
        // TODO: instructions, set unwind IP
        call_frame->next_ip();
      } else {
        call_frame->ret_ip();
      }

      // TODO: instructions, add instruction
      // call_frame->next_ip(instructions::data_run_exception.width);
      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
