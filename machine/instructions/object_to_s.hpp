  intptr_t literal = next_int;
  if(!kind_of<String>(stack_top())) {
    CallSite* call_site = reinterpret_cast<CallSite*>(literal);

    Arguments args(call_site->name(), stack_top(), cNil, 0, 0);
    Object* ret = call_site->execute(state, args);
    if(ret && !kind_of<String>(ret)) {
      ret = stack_top()->to_s(state, false);
    }

    (void)stack_pop();
    CHECK_AND_PUSH(ret);

    state->vm()->checkpoint(state);
  }
