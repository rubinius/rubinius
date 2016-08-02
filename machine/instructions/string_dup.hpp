  String *s1 = as<String>(stack_pop());
  String *dup = s1->string_dup(state);

  if(!dup) {
    state->raise_exception(
        Exception::make_exception(state, Exception::get_runtime_error(state),
            "unable to allocate String from string_dup"));
    return static_cast<intptr_t>(NULL);
  }

  stack_push(dup);
