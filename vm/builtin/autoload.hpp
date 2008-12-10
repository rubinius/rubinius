#ifndef RBX_BUILTIN_AUTOLOAD_HPP
#define RBX_BUILTIN_AUTOLOAD_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {

  class Autoload : public Object {
  public:
    const static object_type type = AutoloadType;

    /**  Register class with the VM. */
    static void   init(STATE);

    // Ruby.primitive :autoload_allocate
    static Autoload* create(STATE);

  public:   /* TypeInfo */

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
    };
  };

}

#endif
