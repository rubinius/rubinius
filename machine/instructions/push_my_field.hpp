#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_my_field(STATE, CallFrame* call_frame, intptr_t index) {
      stack_push(call_frame->self()->get_field(state, index));
    }
  }
}
