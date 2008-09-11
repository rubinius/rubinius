#include "vm.hpp"
#include "prelude.hpp"
#include "builtin/class.hpp"
#include "builtin/staticscope.hpp"

namespace rubinius {
  void StaticScope::init(STATE) {
    GO(staticscope).set(state->new_class("StaticScope", G(object),
          StaticScope::fields));
    G(staticscope)->set_object_type(state, StaticScopeType);
  }

  StaticScope* StaticScope::create(STATE) {
    return (StaticScope*)state->new_object(G(staticscope));
  }
}
