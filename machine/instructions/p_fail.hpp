#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline intptr_t p_fail(STATE, CallFrame* call_frame) {
      return 0;
    }
  }
}
