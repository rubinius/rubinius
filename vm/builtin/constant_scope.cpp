#include "builtin/class.hpp"
#include "builtin/constant_scope.hpp"
#include "builtin/system.hpp"
#include "call_frame.hpp"
#include "ontology.hpp"

namespace rubinius {
  void ConstantScope::init(STATE) {
    GO(constantscope).set(ontology::new_class(state,
          "ConstantScope", G(object), G(rubinius)));
    G(constantscope)->set_object_type(state, ConstantScopeType);
  }

  void ConstantScope::bootstrap_methods(STATE) {
    GCTokenImpl gct;
    System::attach_primitive(state, gct,
                             G(constantscope), false,
                             state->symbol("const_set"),
                             state->symbol("constant_scope_const_set"));
  }

  ConstantScope* ConstantScope::create(STATE) {
    return state->new_object<ConstantScope>(G(constantscope));
  }

  Module* ConstantScope::for_method_definition() {
    if(current_module_->nil_p()) {
      return module_;
    }

    return current_module_;
  }

  ConstantScope* ConstantScope::of_sender(STATE, CallFrame* calling_environment) {
    if(calling_environment->previous) {
      return calling_environment->previous->top_ruby_frame()->constant_scope();
    }

    return nil<ConstantScope>();
  }

  Object* ConstantScope::const_set(STATE, Symbol* name, Object* value) {
    module_->set_const(state, name, value);
    return value;
  }

  Object* ConstantScope::cvar_get(STATE, Symbol* name) {
    if(!name->is_cvar_p(state)->true_p()) return Primitives::failure();
    return module_->cvar_get(state, name);
  }

  Object* ConstantScope::cvar_defined(STATE, Symbol* name) {
    if(!name->is_cvar_p(state)->true_p()) return Primitives::failure();

    return module_->cvar_defined(state, name);
  }

  Object* ConstantScope::cvar_set(STATE, Symbol* name, Object* value) {
    if(!name->is_cvar_p(state)->true_p()) return Primitives::failure();

    return module_->cvar_set(state, name, value);
  }

  Object* ConstantScope::cvar_get_or_set(STATE, Symbol* name, Object* value) {
    if(!name->is_cvar_p(state)->true_p()) return Primitives::failure();

    return module_->cvar_get_or_set(state, name, value);
  }
}
