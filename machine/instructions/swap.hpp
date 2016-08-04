#include "interpreter/instructions.hpp"

inline void rubinius::instruction_swap(CallFrame* call_frame) {
  Object* t1 = stack_pop();
  Object* t2 = stack_pop();
  stack_push(t1);
  stack_push(t2);
}
