#include "instructions.hpp"

#include "class/unwind_state.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_exception_state(STATE, CallFrame* call_frame) {
      stack_push(state->unwind_state()->copy(state));
    }
  }
}
