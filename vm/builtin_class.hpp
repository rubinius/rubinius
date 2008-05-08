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
    void setup(STATE, char* name, Module* under = NULL);
    void setup(STATE, SYMBOL name, Module* under = NULL);
    void set_const(STATE, OBJECT sym, OBJECT val);
    void set_const(STATE, char* name, OBJECT val);
    OBJECT get_const(STATE, SYMBOL sym);
    OBJECT get_const(STATE, SYMBOL sym, bool* found);
    OBJECT get_const(STATE, char* sym);

    void set_name(STATE, Module* under, SYMBOL name);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void set_field(STATE, OBJECT target, size_t index, OBJECT val);
      virtual OBJECT get_field(STATE, OBJECT target, size_t index);
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

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void set_field(STATE, OBJECT target, size_t index, OBJECT val);
      virtual OBJECT get_field(STATE, OBJECT target, size_t index);
    };
  };

  /* See t1 */
  template <>
    static bool kind_of<Module>(OBJECT obj) {
      return obj->reference_p() &&
        (obj->obj_type == Module::type ||
         kind_of<Class>(obj));
    }

  class MetaClass : public Class {
    public:
    const static size_t fields = 10;
    const static object_type type = MetaclassType;

    OBJECT attached_instance; // slot

    static MetaClass* attach(STATE, OBJECT obj, OBJECT sup = NULL);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void set_field(STATE, OBJECT target, size_t index, OBJECT val);
      virtual OBJECT get_field(STATE, OBJECT target, size_t index);
    };
  };

  class IncludedModule : public Module {
    public:
    const static size_t field = 6;
    const static object_type type = IncModType;

    OBJECT module; // slot

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void set_field(STATE, OBJECT target, size_t index, OBJECT val);
      virtual OBJECT get_field(STATE, OBJECT target, size_t index);
    };
  };

};

#endif
