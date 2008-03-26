#ifndef RBX_BUILTIN_CLASS_HPP
#define RBX_BUILTIN_CLASS_HPP

#include "prelude.hpp"
#include "object.hpp"

namespace rubinius {
  class Module : public BuiltinType {
    public:
    const static size_t fields = 7;
    OBJECT instance_variables;
    OBJECT method_table;
    OBJECT method_cache;
    OBJECT name;
    OBJECT constants;
    OBJECT encloser;
    OBJECT superclass;

    void setup(STATE);
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
