  intptr_t literal = next_int;
  intptr_t count = next_int;
  Object* block = stack_pop();
  CallSite* call_site = reinterpret_cast<CallSite*>(literal);
  Object* const recv = call_frame->self();

  Arguments new_args(call_site->name(), recv, block, count,
                     stack_back_position(count));

  Object* ret = call_site->execute(state, new_args);

  stack_clear(count);

  CHECK_AND_PUSH(ret);

  state->vm()->checkpoint(state);
