  intptr_t literal = next_int;
  Object* val = stack_top();
  if(val->nil_p()) {
    stack_set_top(reinterpret_cast<Object*>(literal));
  } else {
    // TODO: instructions
    // store_literal(reinterpret_cast<opcode>(val));
  }
