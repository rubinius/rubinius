  intptr_t location = next_int;
  Object* t1 = stack_pop();
  if(t1->nil_p()) {
    store_ip(location);
  }
