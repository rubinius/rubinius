#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_stack_local(CallFrame* call_frame, intptr_t which) {
      stack_push(stack_local(which));
    }
  }
}
