#include "interpreter/instructions.hpp"

inline void rubinius::instruction_push_scope(CallFrame* call_frame) {
  stack_push(call_frame->lexical_scope());
}
