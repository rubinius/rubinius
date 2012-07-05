#ifndef RBX_BUILTIN_AUTOLOAD_HPP
#define RBX_BUILTIN_AUTOLOAD_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  struct CallFrame;

  class Autoload : public Object {
  public:
    const static object_type type = AutoloadType;

  private:
    Symbol* name_; // slot
    Module* scope_; // slot
    Object* path_; // slot

  public:
    attr_accessor(name, Symbol);
    attr_accessor(scope, Module);
    attr_accessor(path, Object);

    /**  Register class with the VM. */
    static void   init(STATE);

    // Rubinius.primitive :autoload_allocate
    static Autoload* create(STATE);

    Object* resolve(STATE, GCToken gct, CallFrame* call_frame, Module* under, bool honor_require = false);
    Object* resolve(STATE, GCToken gct, CallFrame* call_frame, bool honor_require = false);

  public:   /* TypeInfo */

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo);
    };
  };

}

#endif
