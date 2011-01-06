#ifndef RBX_VM_BUILTIN_BASICOBJECT_HPP
#define RBX_VM_BUILTIN_BASICOBJECT_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class Class;
  class Object;

  class BasicObject : public Object {
  public:   /* Slots and bookkeeping */

    /** Class type identifier. */
    static const object_type type = BasicObjectType;

  public:   /* accessors */

  };

}

#endif
