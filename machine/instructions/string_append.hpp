#include "interpreter/instructions.hpp"

#include "builtin/string.hpp"

inline void rubinius::instruction_string_append(STATE, CallFrame* call_frame) {
  String* s1 = as<String>(stack_pop());
  String* s2 = as<String>(stack_pop());
  s1->append(state, s2);
  stack_push(s1);
}
