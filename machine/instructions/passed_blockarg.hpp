  intptr_t count = next_int;
  if(!call_frame->arguments) {
    Exception::internal_error(state,
                              "no arguments object");
    RUN_EXCEPTION();
  }

  stack_push(RBOOL(count == (int)call_frame->arguments->total()));
