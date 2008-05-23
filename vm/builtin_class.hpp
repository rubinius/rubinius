#ifndef RBX_BUILTIN_CLASS_HPP
#define RBX_BUILTIN_CLASS_HPP

#include "objects.hpp"

namespace rubinius {
  class Module : public BuiltinType {
    public:
    const static size_t fields = 5;
    const static object_type type = ModuleType;

    OBJECT __ivars__; // slot
    LookupTable* method_table; // slot
    SYMBOL name; // slot
    LookupTable* constants; // slot
    Module* superclass; // slot

    static Module* create(STATE);
    void setup(STATE);
    void setup(STATE, const char* name, Module* under = NULL);
    void setup(STATE, SYMBOL name, Module* under = NULL);
    void set_const(STATE, OBJECT sym, OBJECT val);
    void set_const(STATE, const char* name, OBJECT val);
    OBJECT get_const(STATE, SYMBOL sym);
    OBJECT get_const(STATE, SYMBOL sym, bool* found);
    OBJECT get_const(STATE, const char* sym);

    void set_name(STATE, Module* under, SYMBOL name);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

  class Class : public Module {
    public:
    const static size_t fields = 9;
    const static object_type type = ClassType;

    FIXNUM instance_fields; // slot
    OBJECT has_ivars; // slot
    OBJECT needs_cleanup; // slot
    FIXNUM instance_type; // slot

    void set_object_type(size_t type) {
      instance_type = Object::i2n(type);
    }

    static Class* create(STATE, Class* super);

    // Ruby.primitive :class_allocate
    OBJECT allocate(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

  class MetaClass : public Class {
    public:
    const static size_t fields = 10;
    const static object_type type = MetaclassType;

    OBJECT attached_instance; // slot

    static MetaClass* attach(STATE, OBJECT obj, OBJECT sup = NULL);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

  class IncludedModule : public Module {
    public:
    const static size_t field = 6;
    const static object_type type = IncModType;

    OBJECT module; // slot

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

  /* See t1 */
  template <>
    static bool kind_of<Module>(OBJECT obj) {
      return obj->reference_p() &&
        (obj->obj_type == Module::type ||
         obj->obj_type == Class::type ||
         obj->obj_type == MetaClass::type ||
         obj->obj_type == IncludedModule::type);
    }


};

#endif
