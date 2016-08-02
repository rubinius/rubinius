  intptr_t literal = next_int;
  intptr_t count = next_int;
  Object* recv = stack_back(count);
  CallSite* call_site = reinterpret_cast<CallSite*>(literal);

  Arguments args(call_site->name(), recv, cNil, count,
                 stack_back_position(count));

  Object* ret = call_site->execute(state, args);

  stack_clear(count + 1);

  CHECK_AND_PUSH(ret);

  state->vm()->checkpoint(state);
