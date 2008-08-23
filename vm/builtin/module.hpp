#ifndef RBX_BUILTIN_MODULE_HPP
#define RBX_BUILTIN_MODULE_HPP

#include "builtin/object.hpp"
#include "builtin/fixnum.hpp"
#include "type_info.hpp"

namespace rubinius {
  class LookupTable;

  class Module : public Object {
    public:
    const static size_t fields = 4;
    const static object_type type = ModuleType;

    LookupTable* method_table; // slot
    SYMBOL name; // slot
    LookupTable* constants; // slot
    Module* superclass; // slot

    // Ruby.primitive :module_allocate
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

  class IncludedModule : public Module {
    public:
    const static size_t field = Module::fields + 1;
    const static object_type type = IncModType;

    OBJECT module; // slot

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
};

#endif
