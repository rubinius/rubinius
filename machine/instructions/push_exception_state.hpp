#include "interpreter/instructions.hpp"

#include "class/thread_state.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_exception_state(STATE, CallFrame* call_frame) {
      stack_push(state->vm()->thread_state()->state_as_object(state));
    }
  }
}
