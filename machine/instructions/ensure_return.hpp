  state->vm()->thread_state()->raise_return(stack_top(), call_frame->promote_scope(state));
  RUN_EXCEPTION();
