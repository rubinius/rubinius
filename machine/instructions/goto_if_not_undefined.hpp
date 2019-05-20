#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline bool goto_if_not_undefined(STATE, CallFrame* call_frame) {
      Object* t1 = stack_pop();
      if(t1 != G(undefined)) {
        return true;
      } else {
        return false;
      }
    }
  }
}
