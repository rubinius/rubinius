#include "interpreter/instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void reraise(STATE, CallFrame* call_frame) {
      interp_assert(state->vm()->thread_state()->raise_reason() != cNone);
    }
  }
}
