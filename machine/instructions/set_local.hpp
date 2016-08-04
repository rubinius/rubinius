#include "interpreter/instructions.hpp"

inline void rubinius::instruction_set_local(CallFrame* call_frame, intptr_t local) {
  call_frame->scope->set_local(local, stack_top());
}
