#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_local(CallFrame* call_frame, intptr_t local) {
      stack_push(call_frame->scope->get_local(local));
    }
  }
}
