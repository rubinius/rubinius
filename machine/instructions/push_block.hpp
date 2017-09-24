#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_block(STATE, CallFrame* call_frame) {
      stack_push(call_frame->scope->block());
    }
  }
}
