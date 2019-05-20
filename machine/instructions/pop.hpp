#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void pop(CallFrame* call_frame) {
      (void)stack_pop();
    }
  }
}
