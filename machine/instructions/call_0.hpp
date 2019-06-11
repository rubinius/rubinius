#include "instructions.hpp"

#include "class/call_site.hpp"

namespace rubinius {
  namespace instructions {
    inline bool call_0(STATE, CallFrame* call_frame, intptr_t literal) {
      CallSite* call_site = reinterpret_cast<CallSite*>(literal);

      Arguments args(call_site->name(), cNil, cNil, 0, 0);
      Object* value = call_site->execute(state, args);

      CHECK_AND_PUSH(value);
    }
  }
}
