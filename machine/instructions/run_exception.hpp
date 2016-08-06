#include "interpreter/instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline bool run_exception(STATE, CallFrame* call_frame) {
      VMThreadState* th = state->vm()->thread_state();

      switch(th->raise_reason()) {
      case cException:
        if(call_frame->unwinds->has_unwinds()) {
          // TODO: instructions, set this in call_frame
          UnwindInfo info = call_frame->unwinds->pop();
          stack_position(info.stack_depth);
          call_frame->set_ip(info.target_ip);
          return true;
        } else {
          call_frame->scope->flush_to_heap(state);
          return false;
        }

      case cBreak:
        // If we're trying to break to here, we're done!
        if(th->destination_scope() == call_frame->scope->on_heap()) {
          stack_push(th->raise_value());
          th->clear_break();

          /* Don't return here, because we want to loop back to the top and keep
           * running this method.
           */
          // TODO: instructions
          call_frame->next_ip();
          return true;
        }

        // Otherwise, fall through and run the unwinds
      case cReturn:
      case cCatchThrow:
      case cThreadKill:
        // Otherwise, we're doing a long return/break unwind through
        // here. We need to run ensure blocks.
        while(call_frame->unwinds->has_unwinds()) {
          // TODO: instructions, set this in call_frame
          UnwindInfo info = call_frame->unwinds->pop();
          if(info.for_ensure()) {
            stack_position(info.stack_depth);

            // Don't reset ep here, we're still handling the return/break.
            call_frame->set_ip(info.target_ip);
            return true;
          }
        }

        // Ok, no ensures to run.
        if(th->raise_reason() == cReturn) {
          call_frame->scope->flush_to_heap(state);

          // If we're trying to return to here, we're done!
          if(th->destination_scope() == call_frame->scope->on_heap()) {
            Object* val = th->raise_value();
            th->clear_return();
            stack_push(val);
            call_frame->ret_ip();
            // TODO: instructions
            return true;
          } else {
            // Give control of this exception to the caller.
            stack_push(NULL);
            call_frame->ret_ip();
            // TODO: instructions
            return false;
          }

        } else { // Not for us!
          call_frame->scope->flush_to_heap(state);

          // Give control of this exception to the caller.
          stack_push(NULL);
          call_frame->ret_ip();
          // TODO: instructions
          return false;
        }

      case cExit:
        call_frame->scope->flush_to_heap(state);
        stack_push(NULL);
        call_frame->ret_ip();
        // TODO: instructions
        return false;
        break;
      case cFiberCancel:
        stack_push(NULL);
        call_frame->ret_ip();
        // TODO: instructions
        return false;
        break;
      default:
        Exception::interpreter_error(state, "exception handler failed to dispatch");
        stack_push(NULL);
        call_frame->ret_ip();
        // TODO: instructions
        return false;
        break;
      } // switch
    }
  }
}
