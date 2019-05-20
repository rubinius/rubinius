#include "instructions/ret.hpp"
#include "instructions/unwind.hpp"

#include "class/unwind_site.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t unwind(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      switch(instructions::unwind(state, call_frame)) {
        case instructions::cExceptionRescue: {
          UnwindSite* unwind_site = call_frame->pop_unwind();
          stack_position(unwind_site->stack_depth());
          call_frame->set_ip(unwind_site->ip());
          break;
        }
        case instructions::cExceptionEnsure: {
          UnwindSite* unwind_site = call_frame->pop_unwind();
          stack_position(unwind_site->stack_depth());
          call_frame->set_ip(unwind_site->ip());
          break;
        }
        case instructions::cExceptionBreak:
          call_frame->next_ip(instructions::data_unwind.width);
          break;
        case instructions::cExceptionReturn:
          return instructions::ret(state, call_frame);
        case instructions::cExceptionUnwind:
          return 0;
      }

      return ((instructions::Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
