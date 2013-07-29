#ifndef RBX_BUILTIN_MODULE_HPP
#define RBX_BUILTIN_MODULE_HPP

#include "builtin/object.hpp"

namespace rubinius {
  class ConstantTable;
  class MethodTable;

  class Module : public Object {
  public:
    const static object_type type = ModuleType;

  private:
    MethodTable* method_table_;     // slot
    Symbol* module_name_;           // slot
    ConstantTable* constant_table_; // slot
    Module* superclass_;            // slot
    Module* origin_;                // slot
    Array* seen_ivars_;             // slot
    Class* mirror_;                 // slot
    Array* hierarchy_subclasses_;   // slot

  public:
    /* accessors */

    attr_accessor(method_table, MethodTable);
    attr_accessor(module_name, Symbol);
    attr_accessor(constant_table, ConstantTable);
    attr_accessor(superclass, Module);
    attr_accessor(origin, Module);
    attr_accessor(seen_ivars, Array);
    attr_accessor(mirror, Class);
    attr_accessor(hierarchy_subclasses, Array);

    /* interface */
    static Module* create(STATE);

    static void bootstrap_methods(STATE);

    // Rubinius.primitive :module_allocate
    static Module* allocate(STATE, Object* self);

    void set_name(STATE, Symbol* name, Module* under);
    void set_name(STATE, std::string name, Module* under);

    String* get_name(STATE);

    // Rubinius.primitive :module_const_set
    Object* const_set(STATE, Symbol* name, Object* value);

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

    // Rubinius.primitive :module_mirror
    static Class* mirror(STATE, Object* obj);

    // Rubinius.primitive :module_track_subclass
    Object* track_subclass(STATE, Module* mod);

    void setup(STATE);
    void setup(STATE, std::string name, Module* under = NULL);
    void set_const(STATE, Symbol* sym, Object* val);
    void set_const(STATE, std::string name, Object* val);
    Object* get_const(STATE, Symbol* sym);
    Object* get_const(STATE, Symbol* sym, Symbol* min_vis, ConstantMissingReason* reason, bool check_super=false, bool replace_autoload=false);
    Object* get_const(STATE, std::string sym);

    void del_const(STATE, Symbol* sym);

    void add_method(STATE, Symbol* name, Executable* exec, Symbol* vis = 0);

    Object* reset_method_cache(STATE, Symbol* name);

    Executable* find_method(Symbol* name, Module** defined_in = 0);

    std::string debug_str(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, Object* self, int level);
      virtual void mark(Object* obj, ObjectMark& mark);
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
