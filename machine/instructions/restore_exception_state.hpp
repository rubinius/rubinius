#include "interpreter/instructions.hpp"

#include "class/thread_state.hpp"

namespace rubinius {
  namespace instructions {
    inline void restore_exception_state(STATE, CallFrame* call_frame) {
      Object* top = stack_pop();
      if(top->nil_p()) {
        state->vm()->thread_state()->clear();
      } else {
        state->vm()->thread_state()->set_state(state, as<ThreadState>(top));
      }
    }
  }
}
