#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_tagged_nil(CallFrame* call_frame, intptr_t nil) {
      stack_push(reinterpret_cast<Object*>(nil));
    }
  }
}
