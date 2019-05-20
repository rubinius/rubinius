#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void dup_many(CallFrame* call_frame, intptr_t count) {
      Object** objs = stack_back_position(count);
      for(intptr_t i = 0; i < count; i++) {
        stack_push(objs[i]);
      }
    }
  }
}
