  Object* value = stack_top();

  if(value->reference_p() && value->is_frozen_p()) {
    Exception::frozen_error(state, value);
    RUN_EXCEPTION();
  }
