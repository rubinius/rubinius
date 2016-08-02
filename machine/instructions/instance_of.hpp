  Object* t1 = stack_pop();
  Class* cls = as<Class>(stack_pop());
  stack_push(RBOOL(t1->class_object(state) == cls));
