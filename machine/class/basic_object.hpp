#ifndef RBX_VM_BUILTIN_BASICOBJECT_HPP
#define RBX_VM_BUILTIN_BASICOBJECT_HPP

#include "class/object.hpp"

namespace rubinius {
  class BasicObject : public Object {
  public:
    static const object_type type = BasicObjectType;

    static void bootstrap(STATE);
  };
}

#endif
