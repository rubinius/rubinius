#include "instructions.hpp"

#include "class/call_site.hpp"

namespace rubinius {
  namespace instructions {
    inline bool send_method(STATE, CallFrame* call_frame, intptr_t literal) {
      Object* recv = stack_top();
      CallSite* call_site = reinterpret_cast<CallSite*>(literal);

      Arguments args(call_site->name(), recv, cNil, 0, 0);

      (void)stack_pop();

      Object* value = call_site->execute(state, args);

      CHECK_AND_PUSH(value);
    }
  }
}
