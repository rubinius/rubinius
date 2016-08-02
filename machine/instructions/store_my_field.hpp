  intptr_t index = next_int;
  if(CBOOL(call_frame->self()->frozen_p(state))) {
    Exception::frozen_error(state, call_frame->self());
    RUN_EXCEPTION();
  }
  call_frame->self()->set_field(state, index, stack_top());
