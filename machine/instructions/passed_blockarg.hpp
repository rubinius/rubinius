#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline bool passed_blockarg(STATE, CallFrame* call_frame, intptr_t count) {
      if(!call_frame->arguments) {
        Exception::internal_error(state, "no arguments object");
        return false;
      }

      stack_push(RBOOL(count == (int)call_frame->arguments->total()));
      return true;
    }
  }
}
