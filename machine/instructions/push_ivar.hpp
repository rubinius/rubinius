#include "interpreter/instructions.hpp"

inline bool rubinius::instruction_push_ivar(STATE, CallFrame* call_frame, intptr_t literal) {
  Symbol* sym = reinterpret_cast<Symbol*>(literal);
  Object* ret = call_frame->self()->get_ivar(state, sym);

  CHECK_AND_PUSH(ret);
}
