#ifndef RBX_VM_BUILTIN_BASICOBJECT_HPP
#define RBX_VM_BUILTIN_BASICOBJECT_HPP

#include "builtin/object.hpp"

namespace rubinius {
  class BasicObject : public Object {
  public:   /* Slots and bookkeeping */

    /** Class type identifier. */
    static const object_type type = BasicObjectType;

    static void init(STATE);

  public:   /* accessors */

  };

}

#endif
