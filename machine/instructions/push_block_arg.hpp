  if(!call_frame->arguments) {
    Exception::internal_error(state,
                              "no arguments object");
    RUN_EXCEPTION();
  }

  stack_push(call_frame->arguments->block());
