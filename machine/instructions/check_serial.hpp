#include "instructions.hpp"

#include "class/call_site.hpp"

namespace rubinius {
  namespace instructions {
    inline void check_serial(STATE, CallFrame* call_frame, intptr_t literal, intptr_t serial) {
      Object* recv = stack_pop();
      CallSite* call_site = reinterpret_cast<CallSite*>(literal);

      stack_push(RBOOL(call_site->valid_serial_p(state, recv, G(sym_public), serial)));
    }
  }
}
