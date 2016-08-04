#include "interpreter/instructions.hpp"

inline void rubinius::instruction_set_const_at(STATE, CallFrame* call_frame, intptr_t literal) {
  Symbol* sym = reinterpret_cast<Symbol*>(literal);
  Object* val = stack_pop();
  Module* under = as<Module>(stack_pop());

  under->set_const(state, sym, val);
  stack_push(val);
}
