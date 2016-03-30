#include "memory.hpp"
#include "call_frame.hpp"

#include "builtin/class.hpp"
#include "builtin/constant_scope.hpp"
#include "builtin/system.hpp"

namespace rubinius {
  void ConstantScope::bootstrap(STATE) {
    GO(constantscope).set(state->memory()->new_class<Class, ConstantScope>(
          state, G(rubinius), "ConstantScope"));
  }

  void ConstantScope::bootstrap_methods(STATE) {
    System::attach_primitive(state,
                             G(constantscope), false,
                             state->symbol("const_set"),
                             state->symbol("constant_scope_const_set"));
  }

  ConstantScope* ConstantScope::create(STATE) {
    return state->memory()->new_object<ConstantScope>(state, G(constantscope));
  }

  ConstantScope* ConstantScope::of_sender(STATE) {
    if(CallFrame* frame = state->vm()->get_ruby_frame(1)) {
      return frame->constant_scope();
    }

    return nil<ConstantScope>();
  }

  Object* ConstantScope::const_set(STATE, Symbol* name, Object* value) {
    module()->set_const(state, name, value);
    return value;
  }

  Object* ConstantScope::cvar_get(STATE, Symbol* name) {
    if(!name->is_cvar_p(state)->true_p()) return Primitives::failure();
    return module()->cvar_get(state, name);
  }

  Object* ConstantScope::cvar_defined(STATE, Symbol* name) {
    if(!name->is_cvar_p(state)->true_p()) return Primitives::failure();

    return module()->cvar_defined(state, name);
  }

  Object* ConstantScope::cvar_set(STATE, Symbol* name, Object* value) {
    if(!name->is_cvar_p(state)->true_p()) return Primitives::failure();

    return module()->cvar_set(state, name, value);
  }

  Object* ConstantScope::cvar_get_or_set(STATE, Symbol* name, Object* value) {
    if(!name->is_cvar_p(state)->true_p()) return Primitives::failure();

    return module()->cvar_get_or_set(state, name, value);
  }

  void ConstantScope::Info::show(STATE, Object* self, int level) {
    ConstantScope* scope = as<ConstantScope>(self);

    class_header(state, self);
    indent_attribute(++level, "module"); scope->module()->show(state, level);
    indent_attribute(level, "current_module"); scope->current_module()->show(state, level);
    indent_attribute(level, "parent"); scope->parent()->show(state, level);
    close_body(level);
  }
}
