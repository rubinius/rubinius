#include "interpreter/instructions.hpp"

inline void rubinius::instruction_pop_many(CallFrame* call_frame, intptr_t count) {
  stack_clear(count);
}
