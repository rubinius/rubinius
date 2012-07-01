#include "prelude.hpp"
#include "call_frame.hpp"
#include "builtin/class.hpp"
#include "builtin/autoload.hpp"
#include "helpers.hpp"

#include "ontology.hpp"
#include "on_stack.hpp"

namespace rubinius {
  Autoload* Autoload::create(STATE) {
    return state->new_object<Autoload>(G(autoload));
  }

  void Autoload::init(STATE) {
    GO(autoload).set(ontology::new_class(state, "Autoload"));
    G(autoload)->set_object_type(state, AutoloadType);
  }

  Object* Autoload::resolve(STATE, CallFrame* call_frame, Module* under, bool honor_require) {
    Autoload* self = this;
    OnStack<1> os(state, self);
    Object* res = send(state, call_frame, state->symbol("resolve"));

    if(!res) return NULL;

    if(CBOOL(res) || !honor_require) {
      bool found;
      Object* constant = Helpers::const_get_under(state, under, self->name(), &found, self);

      if(!constant) return NULL;

      if(found) {
        return constant;
      }
      return Helpers::const_missing_under(state, under, self->name(), call_frame);
    }
    return cNil;
  }

  Object* Autoload::resolve(STATE, CallFrame* call_frame, bool honor_require) {
    Autoload* self = this;
    OnStack<1> os(state, self);
    Object* res = send(state, call_frame, state->symbol("resolve"));

    if(!res) return NULL;

    if(CBOOL(res) || !honor_require) {
      bool found;
      Object* constant = Helpers::const_get(state, call_frame, self->name(), &found, self);

      if(!constant) return NULL;

      if(found) {
        return constant;
      }
      return Helpers::const_missing(state, self->name(), call_frame);
    }
    return cNil;
  }
}
