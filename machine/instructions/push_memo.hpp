#include "interpreter/instructions.hpp"

inline void rubinius::instruction_push_memo(CallFrame* call_frame, intptr_t literal) {
  Object* val = stack_top();
  if(val->nil_p()) {
    stack_set_top(reinterpret_cast<Object*>(literal));
  } else {
    // TODO: instructions
    // store_literal(reinterpret_cast<opcode>(val));
  }
}
