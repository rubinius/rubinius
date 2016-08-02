  intptr_t literal = next_int;
  Symbol* sym = reinterpret_cast<Symbol*>(literal);
  Object* ret = call_frame->self()->get_ivar(state, sym);

  CHECK_AND_PUSH(ret);
