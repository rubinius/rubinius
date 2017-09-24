#include "instructions.hpp"

#include "class/string.hpp"

namespace rubinius {
  namespace instructions {
    inline bool string_dup(STATE, CallFrame* call_frame) {
      String *s1 = as<String>(stack_pop());
      String *dup = s1->string_dup(state);

      if(!dup) {
        state->raise_exception(
            Exception::make_exception(state, Exception::get_runtime_error(state),
                "unable to allocate String from string_dup"));
        return false;
      }

      stack_push(dup);
      return true;
    }
  }
}
