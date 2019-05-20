#ifndef RBX_BUILTIN_AUTOLOAD_HPP
#define RBX_BUILTIN_AUTOLOAD_HPP

#include "object_utils.hpp"

#include "class/module.hpp"
#include "class/object.hpp"
#include "class/symbol.hpp"
#include "class/thread.hpp"

namespace rubinius {
  struct CallFrame;
  class Thread;

  class Autoload : public Object {
  public:
    const static object_type type = AutoloadType;

    attr_accessor(name, Symbol);
    attr_accessor(scope, Module);
    attr_accessor(path, Object);
    attr_accessor(constant, Object);
    attr_accessor(thread, Thread);
    attr_accessor(loading, Object);
    attr_accessor(loaded, Object);

    /**  Register class with the VM. */
    static void bootstrap(STATE);
    static void initialize(STATE, Autoload* obj) {
      obj->name(nil<Symbol>());
      obj->scope(nil<Module>());
      obj->path(nil<Object>());
      obj->constant(nil<Object>());
      obj->thread(nil<Thread>());
      obj->loading(nil<Object>());
      obj->loaded(nil<Object>());
    }

    // Rubinius.primitive :autoload_allocate
    static Autoload* create(STATE);

    Object* resolve(STATE, Module* under, bool honor_require = false);
    Object* resolve(STATE, bool honor_require = false);

  public:   /* TypeInfo */

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo);
    };
  };

}

#endif
