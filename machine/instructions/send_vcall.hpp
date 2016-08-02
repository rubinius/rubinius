  intptr_t literal = next_int;
  Object* recv = call_frame->self();
  CallSite* call_site = reinterpret_cast<CallSite*>(literal);

  Arguments args(call_site->name(), recv, cNil, 0, 0);
  Object* ret = call_site->execute(state, args);

  CHECK_AND_PUSH(ret);

  state->vm()->checkpoint(state);
