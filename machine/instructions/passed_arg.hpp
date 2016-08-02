  intptr_t index = next_int;
  if(!call_frame->arguments) {
    Exception::internal_error(state,
                              "no arguments object");
    RUN_EXCEPTION();
  }

  stack_push(RBOOL(index < (int)call_frame->arguments->total() - call_frame->machine_code->post_args));
