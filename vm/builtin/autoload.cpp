#include "helpers.hpp"
#include "object_memory.hpp"
#include "on_stack.hpp"

#include "builtin/autoload.hpp"
#include "builtin/class.hpp"

namespace rubinius {
  void Autoload::bootstrap(STATE) {
    GO(autoload).set(state->memory()->new_class<Class, Autoload>(state, "Autoload"));
  }

  Autoload* Autoload::create(STATE) {
    return state->memory()->new_object<Autoload>(state, G(autoload));
  }

  Object* Autoload::resolve(STATE, CallFrame* call_frame, Module* under, bool honor_require) {
    Autoload* self = this;
    OnStack<1> os(state, self);
    Object* res = send(state, call_frame, state->symbol("resolve"));

    if(!res) return NULL;

    if(CBOOL(res) || !honor_require) {
      ConstantMissingReason reason = vNonExistent;
      Object* constant = Helpers::const_get_under(state, under, self->name(), &reason, self, true);

      if(!constant) return NULL;

      if(reason == vFound) {
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
      ConstantMissingReason reason = vNonExistent;
      Object* constant = Helpers::const_get(state, call_frame, self->name(), &reason, self, true);

      if(!constant) return NULL;

      if(reason == vFound) {
        return constant;
      }
      return Helpers::const_missing(state, self->name(), call_frame);
    }
    return cNil;
  }
}
