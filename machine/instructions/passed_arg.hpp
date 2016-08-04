#include "interpreter/instructions.hpp"

inline bool rubinius::instruction_passed_arg(STATE, CallFrame* call_frame, intptr_t index) {
  if(!call_frame->arguments) {
    Exception::internal_error(state, "no arguments object");
    return false;
  }

  stack_push(RBOOL(index < (int)call_frame->arguments->total() - call_frame->machine_code->post_args));
  return true;
}
