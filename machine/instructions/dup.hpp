#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void dup(CallFrame* call_frame) {
      stack_push(stack_top());
    }
  }
}
