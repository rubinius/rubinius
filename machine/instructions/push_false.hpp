#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_false(CallFrame* call_frame) {
      stack_push(cFalse);
    }
  }
}
