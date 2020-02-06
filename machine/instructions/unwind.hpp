#include "raise_reason.hpp"
#include "instructions.hpp"

#include "class/unwind_site.hpp"
#include "class/unwind_state.hpp"

namespace rubinius {
  namespace instructions {
    inline ExceptionContinuation unwind(STATE, CallFrame* call_frame) {
      UnwindState* unwind = state->unwind_state();

      switch(unwind->raise_reason()) {
      case cException:
        if(UnwindSite* unwind_site = call_frame->unwind) {
          if(unwind_site->unwind_type() == UnwindSite::eRescue) {
            return cExceptionRescue;
          } else if(unwind_site->unwind_type() == UnwindSite::eEnsure) {
            return cExceptionEnsure;
          }
        }
        call_frame->scope->flush_to_heap(state);
        break;

      case cBreak:
        // If we're trying to break to here, we're done!
        if(unwind->destination_scope() == call_frame->scope->on_heap()) {
          stack_push(unwind->raise_value());
          unwind->clear_break();

          // Don't return here, because we want to continue running this method.
          return cExceptionBreak;
        }

        // Fall through.

      case cReturn:
      case cCatchThrow:
      case cThreadKill:
        // Nonlocal return, run ensure blocks.
        while(UnwindSite* unwind_site = call_frame->unwind) {
          if(unwind_site->unwind_type() == UnwindSite::eEnsure) {
            return cExceptionEnsure;
          } else {
            call_frame->pop_unwind();
          }
        }

        // Ok, no ensures to run.
        if(unwind->raise_reason() == cReturn) {
          call_frame->scope->flush_to_heap(state);

          // If we're trying to return to here, we're done!
          if(unwind->destination_scope() == call_frame->scope->on_heap()) {
            stack_push(unwind->raise_value());
            unwind->clear_return();
            return cExceptionReturn;
          }
        } else { // Not for us!
          call_frame->scope->flush_to_heap(state);
        }
        break;

      case cSystemExit:
        call_frame->scope->flush_to_heap(state);
        break;
      case cFiberCancel:
        break;
      case cNone:
        Exception::interpreter_error(state, "no exception available for unwind handler");
        break;
      } // switch

      return cExceptionUnwind;
    }
  }
}
