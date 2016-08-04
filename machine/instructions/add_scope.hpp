#include "interpreter/instructions.hpp"

inline void rubinius::instruction_add_scope(STATE, CallFrame* call_frame) {
  Object* obj = stack_pop();
  Module* mod = as<Module>(obj);
  LexicalScope* scope = LexicalScope::create(state);
  scope->module(state, mod);
  scope->parent(state, call_frame->lexical_scope());
  call_frame->lexical_scope_ = scope;
}
