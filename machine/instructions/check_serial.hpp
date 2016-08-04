#include "interpreter/instructions.hpp"

#include "builtin/call_site.hpp"

inline void rubinius::instruction_check_serial(STATE, CallFrame* call_frame, intptr_t literal, intptr_t serial) {
  Object* recv = stack_pop();
  CallSite* call_site = reinterpret_cast<CallSite*>(literal);

  stack_push(RBOOL(call_site->valid_serial_p(state, recv, G(sym_public), serial)));
}
