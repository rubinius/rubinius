#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void set_stack_local(CallFrame* call_frame, intptr_t which) {
      stack_local(which) = stack_top();
    }
  }
}
