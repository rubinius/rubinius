#include "instructions.hpp"

#include "class/unwind_state.hpp"

namespace rubinius {
  namespace instructions {
    inline void restore_exception_state(STATE, CallFrame* call_frame) {
      Object* top = stack_pop();
      if(top->nil_p()) {
        state->unwind_state()->clear();
      } else {
        state->unwind_state()->set_state(as<UnwindState>(top));
      }
    }
  }
}
