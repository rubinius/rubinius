#include "interpreter/instructions.hpp"

inline bool rubinius::instruction_pop_unwind(STATE, CallFrame* call_frame) {
  // TODO: needs to be in CallFrame
  UnwindInfoSet unwinds;

  if(!unwinds.has_unwinds()) {
    Exception::internal_error(state, "unbalanced pop_unwind");
    return false;
  }

  unwinds.drop();
  return true;
}
