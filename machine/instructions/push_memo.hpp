#include "interpreter/instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void push_memo(CallFrame* call_frame, intptr_t literal) {
      Object* val = stack_top();
      if(val->nil_p()) {
        stack_set_top(reinterpret_cast<Object*>(literal));
      } else {
        // TODO: instructions
        // store_literal(reinterpret_cast<opcode>(val));
      }
    }
  }
}
