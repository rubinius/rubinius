#include "interpreter/instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_current_exception(STATE, CallFrame* call_frame) {
      stack_push(state->vm()->thread_state()->current_exception());
    }
  }
}
