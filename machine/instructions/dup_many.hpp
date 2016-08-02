  intptr_t count = next_int;
  Object** objs = stack_back_position(count);
  for(intptr_t i = 0; i < count; i++) {
    stack_push(objs[i]);
  }
