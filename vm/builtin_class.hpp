#ifndef RBX_BUILTIN_CLASS_HPP
#define RBX_BUILTIN_CLASS_HPP

#include "prelude.hpp"
#include "object.hpp"

namespace rubinius {
  class Module : public BuiltinType {
    public:
    const static size_t fields = 7;
    OBJECT instance_variables;
    LookupTable* method_table;
    OBJECT method_cache;
    OBJECT name;
    LookupTable* constants;
    OBJECT encloser;
    OBJECT superclass;

    void setup(STATE);
    void setup(STATE, char* name, Module* under = NULL);
    void set_const(STATE, OBJECT sym, OBJECT val);
    void set_const(STATE, char* name, OBJECT val);
    OBJECT get_const(STATE, OBJECT sym);
    OBJECT get_const(STATE, char* sym);
  };

  class Class : public Module {
    public:
    const static size_t fields = 11;
    OBJECT instance_fields;
    OBJECT has_ivars;
    OBJECT needs_cleanup;
    OBJECT object_type;

    static bool is_a(OBJECT obj) {
      return obj->obj_type == ClassType;
    }

    void set_object_type(size_t type) {
      object_type = Object::i2n(type);
    }
  };

  class MetaClass : public Class {
    public:
    const static size_t fields = 12;
    OBJECT attached_instance;

    static bool is_a(OBJECT obj) {
      return obj->obj_type == MetaclassType;
    }

    static OBJECT attach(STATE, OBJECT obj, OBJECT sup = NULL);
  };

  class IncludedModule : public Module {
    public:

    OBJECT module;
  };

};

#endif
