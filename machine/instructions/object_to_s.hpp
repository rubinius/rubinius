#include "instructions.hpp"

#include "class/call_site.hpp"
#include "class/string.hpp"

namespace rubinius {
  namespace instructions {
    inline bool object_to_s(STATE, CallFrame* call_frame, intptr_t literal) {
      if(!rubinius::kind_of<String>(stack_top())) {
        CallSite* call_site = reinterpret_cast<CallSite*>(literal);

        Arguments args(call_site->name(), stack_top(), cNil, 0, 0);
        Object* ret = call_site->execute(state, args);
        if(ret && !rubinius::kind_of<String>(ret)) {
          ret = stack_top()->to_s(state, false);
        }

        (void)stack_pop();

        CHECK_AND_PUSH(ret);
      }

      return true;
    }
  }
}
