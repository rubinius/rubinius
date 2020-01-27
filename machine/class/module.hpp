#ifndef RBX_BUILTIN_MODULE_HPP
#define RBX_BUILTIN_MODULE_HPP

#include "object_utils.hpp"

#include "class/object.hpp"

namespace rubinius {
  class LexicalScope;
  class ConstantTable;
  class MethodTable;

  class Module : public Object {
  public:
    const static object_type type = ModuleType;

    attr_accessor(method_table, MethodTable);
    attr_accessor(module_name, Symbol);
    attr_accessor(constant_table, ConstantTable);
    attr_accessor(superclass, Module);
    attr_accessor(origin, Module);
    attr_accessor(seen_ivars, Array);
    attr_accessor(mirror, Class);

    static void bootstrap(STATE);
    static void bootstrap(STATE, Module* mod, Module* under, const char* name);
    static void bootstrap_initialize(STATE, Module* mod, Class* super);

    static void initialize(STATE, Module* mod);
    static void initialize(STATE, Module* mod, const char* name);
    static void initialize(STATE, Module* mod, Module* under, const char* name);
    static void initialize(STATE, Module* mod, Module* under, Symbol* name);

    static Module* create(STATE);

    static void bootstrap_methods(STATE);

    // Rubinius.primitive :module_allocate
    static Module* allocate(STATE, Object* self);

    void set_name(STATE, Symbol* name, Module* under);
    void set_name(STATE, const char* name, Module* under);
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

    void set_const(STATE, Symbol* sym, Object* val);
    void set_const(STATE, std::string name, Object* val);
    Object* get_const(STATE, Symbol* sym);
    Object* get_const(STATE, Symbol* sym, Symbol* min_vis, ConstantMissingReason* reason, bool check_super=false, bool replace_autoload=false);
    Object* get_const(STATE, std::string sym);

    void del_const(STATE, Symbol* sym);

    void add_method(STATE, Symbol* name, String* method_id, Object* exec,
        LexicalScope* scope, Symbol* vis = 0);

    Object* reset_method_cache(STATE, Symbol* name);

    Executable* find_method(STATE, Symbol* name, Module** defined_in = 0);

    std::string debug_str(STATE);

    // std::string name();

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, Object* self, int level);
      virtual void mark(STATE, Object* obj, std::function<void (STATE, Object**)> f);
    };
  };

  class IncludedModule : public Module {
  public:
    const static object_type type = IncludedModuleType;

    attr_accessor(module, Module);

    static void initialize(STATE, IncludedModule* obj, Module* under, const char* name) {
      Module::initialize(state, obj, under, name);

      obj->module(nil<Module>());
    }

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
