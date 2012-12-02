#ifndef RBX_BUILTIN_MODULE_HPP
#define RBX_BUILTIN_MODULE_HPP

#include "builtin/object.hpp"
#include "builtin/fixnum.hpp"
#include "type_info.hpp"

namespace rubinius {
  class LookupTable;
  class MethodTable;
  class IncludedModule;

  class Module : public Object {
  public:
    const static object_type type = ModuleType;

  private:
    MethodTable* method_table_; // slot
    Symbol* module_name_;       // slot
    LookupTable* constant_table_;    // slot
    Module* superclass_;        // slot
    Array* seen_ivars_;         // slot
    IncludedModule* superclass_root_;    // slot

  public:
    /* accessors */

    attr_accessor(method_table, MethodTable);
    attr_accessor(module_name, Symbol);
    attr_accessor(constant_table, LookupTable);
    attr_accessor(superclass, Module);
    attr_accessor(seen_ivars, Array);
    attr_accessor(superclass_root, IncludedModule)

    LookupTable* constants() {
      return constant_table();
    }

    /* interface */
    static Module* create(STATE);

    static void bootstrap_methods(STATE);

    // Rubinius.primitive :module_allocate
    static Module* allocate(STATE, Object* self);

    void set_name(STATE, Symbol* name, Module* under);
    void set_name(STATE, std::string name, Module* under);

    String* get_name(STATE);

    // Rubinius.primitive :module_case_compare
    Object* case_compare(STATE, Object* obj);

    // Rubinius.primitive :module_const_set
    Object* const_set(STATE, Object* name, Object* value);

    // Rubinius.primitive :module_class_variables
    Array* class_variables(STATE);

    // Rubinius.primitive :module_cvar_defined
    Object* cvar_defined(STATE, Symbol* name);

    // Rubinius.primitive :module_cvar_get
    Object* cvar_get(STATE, Symbol* name);

    // Rubinius.primitive :module_cvar_set
    Object* cvar_set(STATE, Symbol* name, Object* val);

    // Rubinius.primitive :module_cvar_get_or_set
    Object* cvar_get_or_set(STATE, Symbol* name, Object* val);

    // Rubinius.primitive :module_cvar_remove
    Object* cvar_remove(STATE, Symbol* name);

    void setup(STATE);
    void setup(STATE, std::string name, Module* under = NULL);
    void set_const(STATE, Object* sym, Object* val);
    void set_const(STATE, std::string name, Object* val);
    Object* get_const(STATE, Symbol* sym);
    Object* get_const(STATE, Symbol* sym, bool* found, bool check_super=false);
    Object* get_const(STATE, std::string sym);

    void del_const(STATE, Symbol* sym);

    void add_method(STATE, GCToken gct, Symbol* name, Executable* exec, Symbol* vis = 0);

    Executable* find_method(Symbol* name, Module** defined_in = 0);

    std::string debug_str(STATE);

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

    // Rubinius.primitive :included_module_allocate
    static IncludedModule* allocate(STATE, Object* self);

    class Info : public Module::Info {
    public:
      BASIC_TYPEINFO(Module::Info)
    };
  };
};

#endif
