#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_mirror(STATE, CallFrame* call_frame) {
      stack_push(G(mirror));
    }
  }
}
