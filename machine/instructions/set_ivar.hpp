  intptr_t literal = next_int;
  if(CBOOL(call_frame->self()->frozen_p(state))) {
    Exception::frozen_error(state, call_frame->self());
    RUN_EXCEPTION();
  }
  Symbol* sym = reinterpret_cast<Symbol*>(literal);
  call_frame->self()->set_ivar(state, sym, stack_top());
