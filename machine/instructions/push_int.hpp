#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_int(CallFrame* call_frame, intptr_t number) {
      stack_push(reinterpret_cast<Fixnum*>(number));
    }
  }
}
