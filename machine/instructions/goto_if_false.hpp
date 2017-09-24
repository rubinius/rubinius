#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline bool goto_if_false(CallFrame* call_frame) {
      Object* t1 = stack_pop();
      if(!CBOOL(t1)) {
        return true;
      } else {
        return false;
      }
    }
  }
}
