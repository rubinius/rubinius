  if(!call_frame->arguments) {
    Exception::internal_error(state,
                              "no arguments object");
    RUN_EXCEPTION();
  }

  Object* obj = call_frame->arguments->block();
  if(CBOOL(obj)) {
    Proc* prc = Proc::from_env(state, G(proc), obj);
    if(!prc) {
      Exception::internal_error(state, "invalid block type");
      RUN_EXCEPTION();
    }
    stack_push(prc);
  } else {
    stack_push(cNil);
  }
