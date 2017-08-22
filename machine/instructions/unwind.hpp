#include "interpreter/instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline ExceptionContinuation unwind(STATE, CallFrame* call_frame, intptr_t ip) {
      VMThreadState* th = state->vm()->thread_state();

      switch(th->raise_reason()) {
      case cException:
        if(call_frame->unwinds->has_unwinds()) {
          call_frame->unwind_info = call_frame->unwinds->pop();
          return cExceptionRescue;
        } else {
          call_frame->scope->flush_to_heap(state);
        }
        break;

      case cBreak:
        // If we're trying to break to here, we're done!
        if(th->destination_scope() == call_frame->scope->on_heap()) {
          stack_push(th->raise_value());
          th->clear_break();

          // Don't return here, because we want to continue running this method.
          return cExceptionBreak;
        }

        // Fall through.

      case cReturn:
      case cCatchThrow:
      case cThreadKill:
        // Nonlocal return, run ensure blocks.
        while(call_frame->unwinds->has_unwinds()) {
          call_frame->unwind_info = call_frame->unwinds->pop();
          if(call_frame->unwind_info.for_ensure()) {
            return cExceptionEnsure;
          }
        }

        // Ok, no ensures to run.
        if(th->raise_reason() == cReturn) {
          call_frame->scope->flush_to_heap(state);

          // If we're trying to return to here, we're done!
          if(th->destination_scope() == call_frame->scope->on_heap()) {
            stack_push(th->raise_value());
            th->clear_return();
            return cExceptionReturn;
          }
        } else { // Not for us!
          call_frame->scope->flush_to_heap(state);
        }
        break;

      case cExit:
        call_frame->scope->flush_to_heap(state);
        break;
      case cFiberCancel:
        break;
      default:
        Exception::interpreter_error(state, "exception handler failed to dispatch");
        break;
      } // switch

      return cExceptionUnwind;
    }
  }
}
