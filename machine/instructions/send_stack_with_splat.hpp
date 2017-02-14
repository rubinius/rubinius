#include "interpreter/instructions.hpp"

#include "class/call_site.hpp"

namespace rubinius {
  namespace instructions {
    inline bool send_stack_with_splat(STATE, CallFrame* call_frame, intptr_t literal, intptr_t count) {
      Object* block = stack_pop();
      Object* ary   = stack_pop();
      Object* recv =  stack_back(count);
      CallSite* call_site = reinterpret_cast<CallSite*>(literal);

      Arguments args(call_site->name(), recv, block, count,
                     stack_back_position(count));

      if(!ary->nil_p()) {
        if(CALL_FLAGS() & CALL_FLAG_CONCAT) {
          args.prepend(state, as<Array>(ary));
        } else {
          args.append(state, as<Array>(ary));
        }
      }

      // TODO: instructions
      // SET_CALL_FLAGS(0);

      stack_clear(count + 1);

      Object* ret = call_site->execute(state, args);

      state->vm()->checkpoint(state);

      CHECK_AND_PUSH(ret);
    }
  }
}
