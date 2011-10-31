#include "vm.hpp"
#include "prelude.hpp"
#include "builtin/class.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/system.hpp"
#include "builtin/symbol.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/compactlookuptable.hpp"

#include "call_frame.hpp"

#include <sstream>

namespace rubinius {
  void StaticScope::init(STATE) {
    GO(staticscope).set(state->new_class("StaticScope", G(object), G(rubinius)));
    G(staticscope)->set_object_type(state, StaticScopeType);
    G(staticscope)->name(state, state->symbol("Rubinius::StaticScope"));
  }

  void StaticScope::bootstrap_methods(STATE) {
    GCTokenImpl gct;
    System::attach_primitive(state, gct,
                             G(staticscope), false,
                             state->symbol("const_set"),
                             state->symbol("static_scope_const_set"));
  }

  StaticScope* StaticScope::create(STATE) {
    return state->new_object<StaticScope>(G(staticscope));
  }

  Module* StaticScope::for_method_definition() {
    if(current_module_->nil_p()) {
      return module_;
    }

    return current_module_;
  }

  StaticScope* StaticScope::of_sender(STATE, CallFrame* calling_environment) {
    if(calling_environment->previous) {
      return calling_environment->previous->top_ruby_frame()->static_scope();
    }

    return nil<StaticScope>();
  }

  Object* StaticScope::const_set(STATE, Object* name, Object* value) {
    module_->set_const(state, name, value);
    return value;
  }

  Object* StaticScope::cvar_get(STATE, Symbol* name) {
    if(!name->is_cvar_p(state)->true_p()) return Primitives::failure();
    return module_->cvar_get(state, name);
  }

  Object* StaticScope::cvar_defined(STATE, Symbol* name) {
    if(!name->is_cvar_p(state)->true_p()) return Primitives::failure();

    return module_->cvar_defined(state, name);
  }

  Object* StaticScope::cvar_set(STATE, Symbol* name, Object* value) {
    if(!name->is_cvar_p(state)->true_p()) return Primitives::failure();

    return module_->cvar_set(state, name, value);
  }

  Object* StaticScope::cvar_get_or_set(STATE, Symbol* name, Object* value) {
    if(!name->is_cvar_p(state)->true_p()) return Primitives::failure();

    return module_->cvar_get_or_set(state, name, value);
  }
}
