#ifndef RBX_BUILTIN_CLASS_HPP
#define RBX_BUILTIN_CLASS_HPP

#include "builtin/object.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/module.hpp"
#include "type_info.hpp"

namespace rubinius {
  class LookupTable;

  class Class : public Module {
    public:
    const static size_t fields = Module::fields + 4;
    const static object_type type = ClassType;

    FIXNUM instance_fields; // slot
    OBJECT has_ivars; // slot
    OBJECT needs_cleanup; // slot
    FIXNUM instance_type; // slot

    void set_object_type(size_t type) {
      instance_type = Fixnum::from(type);
    }

    static Class* create(STATE, Class* super);

    // Ruby.primitive :class_allocate
    OBJECT allocate(STATE);

    class Info : public Module::Info {
    public:
      BASIC_TYPEINFO(Module::Info)
    };
  };

  class MetaClass : public Class {
    public:
    const static size_t fields = Class::fields + 1;
    const static object_type type = MetaclassType;

    OBJECT attached_instance; // slot

    static MetaClass* attach(STATE, OBJECT obj, OBJECT sup = NULL);

    class Info : public Class::Info {
    public:
      BASIC_TYPEINFO(Class::Info)
      virtual void show(STATE, OBJECT self, int level);
    };
  };
};

#endif
