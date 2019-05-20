#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_literal(CallFrame* call_frame, intptr_t literal) {
      stack_push(reinterpret_cast<Object*>(literal));
    }
  }
}
