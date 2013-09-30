#ifndef RBX_BUILTIN_AUTOLOAD_HPP
#define RBX_BUILTIN_AUTOLOAD_HPP

#include "builtin/object.hpp"

namespace rubinius {
  struct CallFrame;
  class Thread;

  class Autoload : public Object {
  public:
    const static object_type type = AutoloadType;

  private:
    Symbol* name_; // slot
    Module* scope_; // slot
    Object* path_; // slot
    Object* constant_; // slot
    Thread* thread_; // slot
    Object* loaded_; // slot

  public:
    attr_accessor(name, Symbol);
    attr_accessor(scope, Module);
    attr_accessor(path, Object);
    attr_accessor(constant, Object);
    attr_accessor(thread, Thread);
    attr_accessor(loaded, Object);

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
