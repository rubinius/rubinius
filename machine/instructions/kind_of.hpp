#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void kind_of(STATE, CallFrame* call_frame) {
      Object* t1 = stack_pop();
      Object* mod = stack_pop();
      stack_push(RBOOL(t1->kind_of_p(state, mod)));
    }
  }
}
