#include "interpreter/instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline intptr_t reraise(STATE, CallFrame* call_frame) {
      interp_assert(state->vm()->thread_state()->raise_reason() != cNone);
      return false;
    }
  }
}
