#include "builtin/basic_object.hpp"
#include "builtin/class.hpp"

namespace rubinius {
  void BasicObject::init(STATE) {
    Class* basic_object = G(basicobject);
    basic_object->set_const(state, state->symbol("BasicObject"), basic_object);
  }
}
