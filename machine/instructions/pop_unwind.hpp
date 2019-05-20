#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void pop_unwind(STATE, CallFrame* call_frame) {
      call_frame->pop_unwind();
    }
  }
}
