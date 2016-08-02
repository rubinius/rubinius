  intptr_t count = next_int;
  Object* t1 = call_frame->scope->block();
  Object* ret;
  Arguments args(G(sym_call), t1, count, stack_back_position(count));

  if(BlockEnvironment *env = try_as<BlockEnvironment>(t1)) {
    ret = env->call(state, args);
  } else if(Proc* proc = try_as<Proc>(t1)) {
    ret = proc->yield(state, args);
  } else if(t1->nil_p()) {
    state->raise_exception(Exception::make_lje(state));
    ret = NULL;
  } else {
    Dispatch dispatch(G(sym_call));
    ret = dispatch.send(state, args);
  }

  stack_clear(count);

  CHECK_AND_PUSH(ret);

  state->vm()->checkpoint(state);
