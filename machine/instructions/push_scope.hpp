#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_scope(CallFrame* call_frame) {
      stack_push(call_frame->lexical_scope());
    }
  }
}
