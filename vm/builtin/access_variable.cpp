#include "vm/builtin/access_variable.hpp"
#include "vm/vmaccess_variable.hpp"
#include "builtin/class.hpp"

namespace rubinius {

  void AccessVariable::init(STATE) {
    GO(access_variable).set(state->new_class("AccessVariable", AccessVariable::fields));
    G(access_variable)->set_object_type(AccessVariableType);
  }

  AccessVariable* AccessVariable::allocate(STATE) {
    AccessVariable* av = (AccessVariable*)state->new_object(G(access_variable));
    av->executable = VMAccessVariable::create(state);
    return av;
  }
}
