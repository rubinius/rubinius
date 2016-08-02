  String* s1 = as<String>(stack_pop());
  String* s2 = as<String>(stack_pop());
  s1->append(state, s2);
  stack_push(s1);
