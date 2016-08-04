#include "interpreter/instructions.hpp"

inline bool rubinius::instruction_goto_if_undefined(STATE, CallFrame* call_frame, intptr_t location) {
  Object* t1 = stack_pop();
  if(t1 == G(undefined)) {
    store_ip(location);
    return true;
  } else {
    return false;
  }
}
