#include "interpreter/instructions.hpp"

inline void rubinius::instruction_push_local(CallFrame* call_frame, intptr_t local) {
  stack_push(call_frame->scope->get_local(local));
}
