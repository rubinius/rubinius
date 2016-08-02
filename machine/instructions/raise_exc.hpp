  Object* t1 = stack_pop();
  Exception* exc = as<Exception>(t1);
  exc->locations(state, Location::from_call_stack(state));
  state->raise_exception(exc);
  RUN_EXCEPTION();
