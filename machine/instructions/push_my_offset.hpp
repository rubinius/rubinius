#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_my_offset(CallFrame* call_frame, intptr_t index) {
      Object* val = *reinterpret_cast<Object**>(
          reinterpret_cast<uintptr_t>(call_frame->self()) + index);
      stack_push(val);
    }
  }
}
