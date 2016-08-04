#include "interpreter/instructions.hpp"

#include "builtin/thread_state.hpp"

inline void rubinius::instruction_restore_exception_state(STATE, CallFrame* call_frame) {
    Object* top = stack_pop();
    if(top->nil_p()) {
      state->vm()->thread_state()->clear();
    } else {
      state->vm()->thread_state()->set_state(state, as<ThreadState>(top));
    }
}
