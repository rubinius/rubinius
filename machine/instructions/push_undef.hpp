#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_undef(STATE, CallFrame* call_frame) {
      stack_push(G(undefined));
    }
  }
}
