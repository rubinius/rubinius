  if(!call_frame->arguments) {
    Exception::internal_error(state,
                              "no arguments object");
    RUN_EXCEPTION();
  }

  int k = call_frame->arguments->total();
  if(k == 0) {
    stack_push(cNil);
  } else if(k == 1) {
    stack_push(call_frame->arguments->get_argument(0));
  } else {
    Array* ary = Array::create(state, k);
    for(int i = 0; i < k; i++) {
      ary->set(state, i, call_frame->arguments->get_argument(i));
    }
    stack_push(ary);
  }
