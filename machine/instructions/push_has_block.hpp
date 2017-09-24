#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_has_block(CallFrame* call_frame) {
      stack_push(RBOOL(CBOOL(call_frame->scope->block())));
    }
  }
}
