#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void pop_many(CallFrame* call_frame, intptr_t count) {
      stack_clear(count);
    }
  }
}
