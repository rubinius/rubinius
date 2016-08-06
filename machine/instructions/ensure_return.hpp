#include "interpreter/instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline intptr_t ensure_return(STATE, CallFrame* call_frame) {
      state->vm()->thread_state()->raise_return(stack_top(), call_frame->promote_scope(state));
      return false;
    }
  }
}
