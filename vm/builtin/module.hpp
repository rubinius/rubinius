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

  private:
    LookupTable* method_table_; // slot
    SYMBOL name_;               // slot
    LookupTable* constants_;    // slot
    Module* superclass_;        // slot

  public:
    /* accessors */

    attr_accessor(method_table, LookupTable);
    attr_accessor(name, Symbol);
    attr_accessor(constants, LookupTable);
    attr_accessor(superclass, Module);

    /* interface */
    static Module* create(STATE);

    // Ruby.primitive :module_new_instance
    static Module* new_instance(STATE, OBJECT self); 

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
      virtual void show(STATE, OBJECT self, int level);
    };
  };

  class IncludedModule : public Module {
  public:
    const static size_t fields = Module::fields + 1;
    const static object_type type = IncludedModuleType;

  private:
    OBJECT module_; // slot

  public:
    /* accessors */

    attr_accessor(module, Object);

    /* interface */

    class Info : public Module::Info {
    public:
      BASIC_TYPEINFO(Module::Info)
    };
  };
};

#endif
