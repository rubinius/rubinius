#ifndef RBX_BUILTIN_MODULE_HPP
#define RBX_BUILTIN_MODULE_HPP

#include "builtin/object.hpp"
#include "builtin/fixnum.hpp"
#include "type_info.hpp"

namespace rubinius {
  class LookupTable;

  class Module : public Object {
  public:
    const static object_type type = ModuleType;

  private:
    LookupTable* method_table_; // slot
    Symbol* name_;               // slot
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

    // Ruby.primitive :module_allocate
    static Module* allocate(STATE, Object* self);

    void setup(STATE);
    void setup(STATE, const char* name, Module* under = NULL);
    void setup(STATE, Symbol* name, Module* under = NULL);
    void set_const(STATE, Object* sym, Object* val);
    void set_const(STATE, const char* name, Object* val);
    Object* get_const(STATE, Symbol* sym);
    Object* get_const(STATE, Symbol* sym, bool* found);
    Object* get_const(STATE, const char* sym);

    void set_name(STATE, Module* under, Symbol* name);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, Object* self, int level);
    };
  };

  class IncludedModule : public Module {
  public:
    const static object_type type = IncludedModuleType;

  private:
    Module* module_; // slot

  public:
    /* accessors */

    attr_accessor(module, Module);

    /* interface */
    static IncludedModule* create(STATE);

    // Ruby.primitive :included_module_allocate
    static IncludedModule* allocate(STATE, Object* self);

    class Info : public Module::Info {
    public:
      BASIC_TYPEINFO(Module::Info)
    };
  };
};

#endif
