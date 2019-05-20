#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void set_call_flags(CallFrame* call_frame, intptr_t flags) {
      SET_CALL_FLAGS(flags);
    }
  }
}
