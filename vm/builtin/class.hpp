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
    const static object_type type = ClassType;

  private:
    Fixnum* instance_type_;   // slot

    TypeInfo* type_info_;

  public:
    /* accessors */

    attr_accessor(instance_type, Fixnum);

    TypeInfo* type_info() {
      return type_info_;
    }

    void set_type_info(TypeInfo* ti) {
      type_info_ = ti;
    }

    /* interface */

    /** Returns actual superclass, skipping over IncludedModules */
    Class* direct_superclass(STATE);

    void set_object_type(STATE, size_t type) {
      instance_type(state, Fixnum::from(type));
    }

    static Class* create(STATE, Class* super);

    // Ruby.primitive :class_s_allocate
    static Class* s_allocate(STATE);

    // Ruby.primitive :class_allocate
    Object* allocate(STATE);

    class Info : public Module::Info {
    public:
      BASIC_TYPEINFO(Module::Info)
    };
  };

  class MetaClass : public Class {
  public:
    const static object_type type = MetaClassType;

  private:
    Object* attached_instance_; // slot

  public:
    /* accessors */

    attr_accessor(attached_instance, Object);

    /* interface */

    static MetaClass* attach(STATE, Object* obj, Object* sup = NULL);

    class Info : public Class::Info {
    public:
      BASIC_TYPEINFO(Class::Info)
      virtual void show(STATE, Object* self, int level);
    };
  };
};

#endif
