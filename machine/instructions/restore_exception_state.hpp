    Object* top = stack_pop();
    if(top->nil_p()) {
      state->vm()->thread_state()->clear();
    } else {
      state->vm()->thread_state()->set_state(state, as<ThreadState>(top));
    }
