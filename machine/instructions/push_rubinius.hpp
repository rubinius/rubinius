#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_rubinius(STATE, CallFrame* call_frame) {
      stack_push(G(rubinius));
    }
  }
}
