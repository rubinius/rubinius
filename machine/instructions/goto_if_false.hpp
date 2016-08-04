#include "interpreter/instructions.hpp"

inline bool rubinius::instruction_goto_if_false(CallFrame* call_frame, intptr_t location) {
  Object* t1 = stack_pop();
  if(!CBOOL(t1)) {
    store_ip(location);
    return true;
  } else {
    return false;
  }
}
