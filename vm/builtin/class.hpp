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

  private:
    FIXNUM instance_fields_; // slot
    OBJECT needs_cleanup_;   // slot
    FIXNUM instance_type_;   // slot

  public:
    /* accessors */

    attr_accessor(instance_fields, Fixnum);
    attr_accessor(needs_cleanup, Object);
    attr_accessor(instance_type, Fixnum);

    /* interface */

    void set_object_type(STATE, size_t type) {
      instance_type(state, Fixnum::from(type));
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

  private:
    OBJECT attached_instance_; // slot

  public:
    /* accessors */

    attr_accessor(attached_instance, Object);

    /* interface */

    static MetaClass* attach(STATE, OBJECT obj, OBJECT sup = NULL);

    class Info : public Class::Info {
    public:
      BASIC_TYPEINFO(Class::Info)
      virtual void show(STATE, OBJECT self, int level);
    };
  };
};

#endif
