  intptr_t literal = next_int;
  intptr_t serial = next_int;
  Object* recv = stack_pop();
  CallSite* call_site = reinterpret_cast<CallSite*>(literal);

  stack_push(RBOOL(call_site->valid_serial_p(state,
                    recv, G(sym_public), serial)));
