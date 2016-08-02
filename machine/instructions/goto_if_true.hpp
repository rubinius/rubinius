  intptr_t location = next_int;
  Object* t1 = stack_pop();
  if(CBOOL(t1)) {
    store_ip(location);
  }
