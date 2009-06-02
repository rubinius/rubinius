#include "vm.hpp"
#include "prelude.hpp"
#include "builtin/class.hpp"
#include "builtin/staticscope.hpp"
#include "call_frame.hpp"

namespace rubinius {
  void StaticScope::init(STATE) {
    GO(staticscope).set(state->new_class("StaticScope", G(object), G(rubinius)));
    G(staticscope)->set_object_type(state, StaticScopeType);
    G(staticscope)->name(state, state->symbol("Rubinius::StaticScope"));
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
}
