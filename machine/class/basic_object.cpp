#include "memory.hpp"
#include "object_utils.hpp"

#include "class/basic_object.hpp"
#include "class/class.hpp"

namespace rubinius {
  void BasicObject::bootstrap(STATE) {
    GO(basicobject).set(Class::bootstrap_class(state, nil<Class>(), ObjectType));
  }
}
