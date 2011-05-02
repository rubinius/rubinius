#ifndef RBX_BUILTIN_MODULE_HPP
#define RBX_BUILTIN_MODULE_HPP

#include "builtin/object.hpp"
#include "builtin/fixnum.hpp"
#include "type_info.hpp"

namespace rubinius {
  class LookupTable;
  class MethodTable;

  class Module : public Object {
  public:
    const static object_type type = ModuleType;

  private:
    MethodTable* method_table_; // slot
    Symbol* module_name_;       // slot
    LookupTable* constant_table_;    // slot
    Module* superclass_;        // slot
    Array* seen_ivars_;         // slot

  public:
    /* accessors */

    attr_accessor(method_table, MethodTable);
    attr_accessor(module_name, Symbol);
    attr_accessor(constant_table, LookupTable);
    attr_accessor(superclass, Module);
    attr_accessor(seen_ivars, Array);

    LookupTable* constants() {
      return constant_table();
    }

    Symbol* name() { return module_name_; }
    void name(STATE, Symbol* sym) {
      module_name(state, sym);
    }

    /* interface */
    static Module* create(STATE);

    static void bootstrap_methods(STATE);

    // Ruby.primitive :module_allocate
    static Module* allocate(STATE, Object* self);

    // Ruby.primitive :module_case_compare
    Object* case_compare(STATE, Object* obj);

    // Ruby.primitive :module_const_set
    Object* const_set(STATE, Object* name, Object* value);

    // Ruby.primitive :module_class_variables
    Array* class_variables(STATE);

    // Ruby.primitive :module_cvar_defined
    Object* cvar_defined(STATE, Symbol* name);

    // Ruby.primitive :module_cvar_get
    Object* cvar_get(STATE, Symbol* name);

    // Ruby.primitive :module_cvar_set
    Object* cvar_set(STATE, Symbol* name, Object* val);

    // Ruby.primitive :module_cvar_get_or_set
    Object* cvar_get_or_set(STATE, Symbol* name, Object* val);

    // Ruby.primitive :module_cvar_remove
    Object* cvar_remove(STATE, Symbol* name);

    void setup(STATE);
    void setup(STATE, const char* name, Module* under = NULL);
    void setup(STATE, Symbol* name, Module* under = NULL);
    void set_const(STATE, Object* sym, Object* val);
    void set_const(STATE, const char* name, Object* val);
    Object* get_const(STATE, Symbol* sym);
    Object* get_const(STATE, Symbol* sym, bool* found, bool check_super=false);
    Object* get_const(STATE, const char* sym);

    void del_const(STATE, Symbol* sym);

    void set_name(STATE, Module* under, Symbol* name);

    void add_method(STATE, Symbol* name, Executable* exec, Symbol* vis = 0);

    Executable* find_method(Symbol* name, Module** defined_in = 0);

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
