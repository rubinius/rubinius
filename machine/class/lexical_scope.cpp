#include "memory.hpp"
#include "primitives.hpp"
#include "call_frame.hpp"

#include "class/class.hpp"
#include "class/lexical_scope.hpp"
#include "class/lookup_table.hpp"
#include "class/system.hpp"

namespace rubinius {
  void LexicalScope::bootstrap(STATE) {
    GO(constantscope).set(state->memory()->new_class<Class, LexicalScope>(
          state, G(rubinius), "LexicalScope"));
  }

  void LexicalScope::bootstrap_methods(STATE) {
    System::attach_primitive(state,
                             G(constantscope), false,
                             state->symbol("const_set"),
                             state->symbol("lexical_scope_const_set"));
  }

  LexicalScope* LexicalScope::create(STATE) {
    return state->memory()->new_object<LexicalScope>(state, G(constantscope));
  }

  LexicalScope* LexicalScope::of_sender(STATE) {
    if(CallFrame* frame = state->get_ruby_frame(1)) {
      return frame->lexical_scope();
    }

    return nil<LexicalScope>();
  }

  Object* LexicalScope::const_set(STATE, Symbol* name, Object* value) {
    module()->set_const(state, name, value);
    return value;
  }

  Object* LexicalScope::cvar_get(STATE, Symbol* name) {
    if(!name->is_cvar_p(state)->true_p()) return Primitives::failure();
    return module()->cvar_get(state, name);
  }

  Object* LexicalScope::cvar_defined(STATE, Symbol* name) {
    if(!name->is_cvar_p(state)->true_p()) return Primitives::failure();

    return module()->cvar_defined(state, name);
  }

  Object* LexicalScope::cvar_set(STATE, Symbol* name, Object* value) {
    if(!name->is_cvar_p(state)->true_p()) return Primitives::failure();

    return module()->cvar_set(state, name, value);
  }

  Object* LexicalScope::cvar_get_or_set(STATE, Symbol* name, Object* value) {
    if(!name->is_cvar_p(state)->true_p()) return Primitives::failure();

    return module()->cvar_get_or_set(state, name, value);
  }

  void LexicalScope::Info::show(STATE, Object* self, int level) {
    LexicalScope* scope = as<LexicalScope>(self);

    class_header(state, self);
    indent_attribute(++level, "functions"); scope->functions()->show(state, level);
    indent_attribute(level, "module"); scope->module()->show(state, level);
    indent_attribute(level, "current_module"); scope->current_module()->show(state, level);
    indent_attribute(level, "parent"); scope->parent()->show(state, level);
    close_body(level);
  }
}
