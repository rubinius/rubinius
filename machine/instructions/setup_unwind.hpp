  intptr_t ip_ = next_int;
  intptr_t type = next_int;
  unwinds.push(ip_, stack_calculate_sp(), (UnwindType)type);
