#include "vm.hpp"
#include "prelude.hpp"
#include "builtin/class.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/system.hpp"
#include "call_frame.hpp"

namespace rubinius {
  void StaticScope::init(STATE) {
    GO(staticscope).set(state->new_class("StaticScope", G(object), G(rubinius)));
    G(staticscope)->set_object_type(state, StaticScopeType);
    G(staticscope)->name(state, state->symbol("Rubinius::StaticScope"));
  }

  void StaticScope::bootstrap_methods(STATE) {
    System::attach_primitive(state,
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
      return calling_environment->previous->static_scope();
    }

    return (StaticScope*)Qnil;
  }

  Object* StaticScope::const_set(STATE, Object* name, Object* value) {
    module_->set_const(state, name, value);
    return value;
  }
}
