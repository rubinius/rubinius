#ifndef RBX_BUILTIN_AUTOLOAD_HPP
#define RBX_BUILTIN_AUTOLOAD_HPP

#include "object_utils.hpp"

#include "builtin/module.hpp"
#include "builtin/object.hpp"
#include "builtin/symbol.hpp"
#include "builtin/thread.hpp"

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
    attr_accessor(loaded, Object);

    /**  Register class with the VM. */
    static void bootstrap(STATE);
    static void initialize(STATE, Autoload* obj) {
      obj->name_ = nil<Symbol>();
      obj->scope_ = nil<Module>();
      obj->path_ = nil<Object>();
      obj->constant_ = nil<Object>();
      obj->thread_ = nil<Thread>();
      obj->loaded_ = nil<Object>();
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
