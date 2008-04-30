#ifndef RBX_BUILTIN_CLASS_HPP
#define RBX_BUILTIN_CLASS_HPP

#include "objects.hpp"

namespace rubinius {
  class Module : public BuiltinType {
    public:
    const static size_t fields = 7;
    const static object_type type = ModuleType;

    OBJECT instance_variables;
    LookupTable* method_table;
    OBJECT method_cache;
    SYMBOL name;
    LookupTable* constants;
    OBJECT encloser;
    Class* superclass;

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
  };

  class Class : public Module {
    public:
    const static size_t fields = 11;
    const static object_type type = ClassType;

    FIXNUM instance_fields;
    OBJECT has_ivars;
    OBJECT needs_cleanup;
    FIXNUM instance_type;

    void set_object_type(size_t type) {
      instance_type = Object::i2n(type);
    }

    static Class* create(STATE, Class* super);
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
    const static size_t fields = 12;
    const static object_type type = MetaclassType;

    OBJECT attached_instance;

    static bool is_a(OBJECT obj) {
      return obj->obj_type == MetaclassType;
    }

    static MetaClass* attach(STATE, OBJECT obj, OBJECT sup = NULL);
  };

  class IncludedModule : public Module {
    public:

    OBJECT module;
  };

};

#endif
