#include "object_memory.hpp"
#include "object_utils.hpp"

#include "builtin/basic_object.hpp"
#include "builtin/class.hpp"

namespace rubinius {
  void BasicObject::bootstrap(STATE) {
    GO(basicobject).set(Class::bootstrap_class(state, nil<Class>(), ObjectType));
  }
}
