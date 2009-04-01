#include "prelude.hpp"
#include "call_frame.hpp"
#include "builtin/class.hpp"
#include "builtin/autoload.hpp"

namespace rubinius {
  Autoload* Autoload::create(STATE) {
    return state->new_object<Autoload>(G(autoload));
  }

  void Autoload::init(STATE) {
    GO(autoload).set(state->new_class("Autoload"));
    G(autoload)->set_object_type(state, AutoloadType);
  }

  Object* Autoload::resolve(STATE, CallFrame* call_frame) {
    return send(state, call_frame, G(sym_call));
  }
}
