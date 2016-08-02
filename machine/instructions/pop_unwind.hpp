  if(!unwinds.has_unwinds()) {
    Exception::internal_error(state, "unbalanced pop_unwind");
    RUN_EXCEPTION();
  }
  unwinds.drop();
