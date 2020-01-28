#include "instructions.hpp"

#include "class/unwind_state.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_current_exception(STATE, CallFrame* call_frame) {
      stack_push(state->unwind_state()->current_exception());
    }
  }
}
