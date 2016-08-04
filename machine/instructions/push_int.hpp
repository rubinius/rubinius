#include "interpreter/instructions.hpp"

inline void rubinius::instruction_push_int(CallFrame* call_frame, intptr_t number) {
  stack_push(reinterpret_cast<Fixnum*>(number));
}
