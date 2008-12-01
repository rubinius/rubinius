#include "prelude.hpp"
#include "builtin/class.hpp"
#include "builtin/autoload.hpp"

namespace rubinius {
  Autoload* Autoload::create(STATE) {
    return (Autoload*)state->new_object(G(autoload));
  }

  void Autoload::init(STATE) {
    GO(autoload).set(state->new_class("Autoload"));
    G(autoload)->set_object_type(state, AutoloadType);
  }
}
