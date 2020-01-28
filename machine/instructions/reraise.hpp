#include "instructions.hpp"

#include "class/unwind_state.hpp"

namespace rubinius {
  namespace instructions {
    inline intptr_t reraise(STATE, CallFrame* call_frame) {
      interp_assert(state->unwind_state()->raise_reason() != cNone);
      return false;
    }
  }
}
