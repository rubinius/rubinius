  intptr_t literal = next_int;
  Symbol* sym = reinterpret_cast<Symbol*>(literal);
  call_frame->lexical_scope()->module()->set_const(state, sym, stack_top());
