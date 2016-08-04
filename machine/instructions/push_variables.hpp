#include "interpreter/instructions.hpp"

#include "builtin/variable_scope.hpp"

inline void rubinius::instruction_push_variables(STATE, CallFrame* call_frame) {
  stack_push(call_frame->promote_scope(state));
}
