  intptr_t literal = next_int;
  intptr_t count = next_int;
  InvokePrimitive invoke_primitive = reinterpret_cast<InvokePrimitive>(literal);

  Object* ret = (*invoke_primitive)(state, stack_back_position(count), count);

  stack_clear(count);

  CHECK_AND_PUSH(ret);
