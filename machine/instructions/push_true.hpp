#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_true(CallFrame* call_frame) {
      stack_push(cTrue);
    }
  }
}
