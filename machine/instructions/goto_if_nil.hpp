#include "interpreter/instructions.hpp"

inline bool rubinius::instruction_goto_if_nil(CallFrame* call_frame, intptr_t location) {
  Object* t1 = stack_pop();
  if(t1->nil_p()) {
    store_ip(location);
    return true;
  } else {
    return false;
  }
}
