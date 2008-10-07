#ifndef RBX_BUILTIN_SYSTEM_HPP
#define RBX_BUILTIN_SYSTEM_HPP

#include "vm/object_types.hpp"
#include "vm/type_info.hpp"

#include "builtin/object.hpp"


namespace rubinius {

  class Array;
  class Fixnum;
  class String;


  /**
   *  VM primitives etc.
   */
  class System : public Object {

  public:   /* Bookkeeping */

    static const std::size_t fields = 0;
    static const object_type type = ObjectType;


  public:   /* Primitives */

    /**
     *  System fork()
     */
    // Ruby.primitive :vm_fork
    static Fixnum*  vm_fork(STATE);

    /**
     *  Replace process with given program and args (exec()).
     */
    // Ruby.primitive :vm_exec
    static Object*  vm_exec(STATE, String* path, Array* args);


  public:   /* Type info */

    class Info : public TypeInfo {
    public:
      Info(object_type type, bool cleanup = false) : TypeInfo(type, cleanup) {}
      virtual ~Info() {}
    };

  };

}

#endif
