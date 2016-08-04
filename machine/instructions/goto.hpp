#include "interpreter/instructions.hpp"

inline void rubinius::instruction_goto(CallFrame* call_frame, intptr_t location) {
  store_ip(location);
}
