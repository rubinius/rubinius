#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline bool goto_if_not_nil(CallFrame* call_frame) {
      Object* t1 = stack_pop();
      if(t1->nil_p()) {
        return false;
      } else {
        return true;
      }
    }
  }
}
