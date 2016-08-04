#include "interpreter/instructions.hpp"

inline bool rubinius::instruction_store_my_field(STATE, CallFrame* call_frame, intptr_t index) {
  if(CBOOL(call_frame->self()->frozen_p(state))) {
    Exception::frozen_error(state, call_frame->self());
    return false;
  }
  call_frame->self()->set_field(state, index, stack_top());
  return true;
}
