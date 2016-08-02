  intptr_t location = next_int;
  Object* t1 = stack_pop();
  Object* t2 = stack_pop();
  if(t1 == t2) {
    store_ip(location);
  }
