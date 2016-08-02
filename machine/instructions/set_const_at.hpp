  intptr_t literal = next_int;
  Symbol* sym = reinterpret_cast<Symbol*>(literal);
  Object* val = stack_pop();
  Module* under = as<Module>(stack_pop());

  under->set_const(state, sym, val);
  stack_push(val);
