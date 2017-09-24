#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_self(CallFrame* call_frame) {
      stack_push(call_frame->self());
    }
  }
}
