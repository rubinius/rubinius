  Object* value = stack_top();

  if(!kind_of<Array>(value)) {
    Object* res = Array::to_ary(state, value);
    if(!res) {
      RUN_EXCEPTION();
    } else {
      stack_set_top(res);
    }
  }
