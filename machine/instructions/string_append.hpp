#include "instructions.hpp"

#include "class/string.hpp"

namespace rubinius {
  namespace instructions {
    inline void string_append(STATE, CallFrame* call_frame) {
      String* s1 = as<String>(stack_pop());
      String* s2 = as<String>(stack_pop());
      s1->append(state, s2);
      stack_push(s1);
    }
  }
}
