#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline bool invoke_primitive(STATE, CallFrame* call_frame, intptr_t literal, intptr_t count) {
      InvokePrimitive invoke_primitive = reinterpret_cast<InvokePrimitive>(literal);

      Object* ret = (*invoke_primitive)(state, stack_back_position(count), count);

      stack_clear(count);

      CHECK_AND_PUSH(ret);
    }
  }
}
