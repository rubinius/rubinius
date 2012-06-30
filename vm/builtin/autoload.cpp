#include "prelude.hpp"
#include "call_frame.hpp"
#include "builtin/class.hpp"
#include "builtin/autoload.hpp"

#include "ontology.hpp"

namespace rubinius {
  Autoload* Autoload::create(STATE) {
    return state->new_object<Autoload>(G(autoload));
  }

  void Autoload::init(STATE) {
    GO(autoload).set(ontology::new_class(state, "Autoload"));
    G(autoload)->set_object_type(state, AutoloadType);
  }

  Object* Autoload::resolve(STATE, CallFrame* call_frame, bool honor_require) {
    if(honor_require) {
      Array* args = Array::create(state, 1);
      args->set(state, 0, cTrue);

      return send(state, call_frame, G(sym_call), args);
    } else {
      return send(state, call_frame, G(sym_call));
    }
  }
}
