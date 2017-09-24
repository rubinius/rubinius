#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_cpath_top(STATE, CallFrame* call_frame) {
      stack_push(G(object));
    }
  }
}
