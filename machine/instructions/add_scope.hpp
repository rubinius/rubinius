#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void add_scope(STATE, CallFrame* call_frame) {
      Object* obj = stack_pop();
      Module* mod = as<Module>(obj);
      LexicalScope* scope = LexicalScope::create(state);
      scope->module(state, mod);
      scope->parent(state, call_frame->lexical_scope());
      if(!scope->parent()->nil_p()) {
        scope->functions(state, call_frame->lexical_scope()->functions());
      }
      call_frame->lexical_scope_ = scope;
    }
  }
}
