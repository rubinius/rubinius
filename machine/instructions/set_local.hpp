#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void set_local(CallFrame* call_frame, intptr_t local) {
      call_frame->scope->set_local(local, stack_top());
    }
  }
}
