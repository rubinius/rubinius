#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline bool goto_if_equal(CallFrame* call_frame) {
      Object* t1 = stack_pop();
      Object* t2 = stack_pop();
      if(t1 == t2) {
        return true;
      } else {
        return false;
      }
    }
  }
}
