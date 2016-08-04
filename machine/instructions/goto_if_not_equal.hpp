#include "interpreter/instructions.hpp"

inline bool rubinius::instruction_goto_if_not_equal(CallFrame* call_frame, intptr_t location) {
  Object* t1 = stack_pop();
  Object* t2 = stack_pop();
  if(t1 != t2) {
    store_ip(location);
    return true;
  } else {
    return false;
  }
}
