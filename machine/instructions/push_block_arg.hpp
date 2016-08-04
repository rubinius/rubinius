#include "interpreter/instructions.hpp"

inline bool rubinius::instruction_push_block_arg(STATE, CallFrame* call_frame) {
  if(!call_frame->arguments) {
    Exception::internal_error(state, "no arguments object");
    return false;
  }

  stack_push(call_frame->arguments->block());

  return true;
}
