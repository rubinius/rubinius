#include "configuration.hpp"
#include "memory.hpp"
#include "object_utils.hpp"
#include "on_stack.hpp"
#include "primitives.hpp"

#include "class/array.hpp"
#include "class/autoload.hpp"
#include "class/class.hpp"
#include "class/compact_lookup_table.hpp"
#include "class/lexical_scope.hpp"
#include "class/constant_table.hpp"
#include "class/executable.hpp"
#include "class/lookup_table.hpp"
#include "class/module.hpp"
#include "class/method_table.hpp"
#include "class/string.hpp"
#include "class/symbol.hpp"
#include "class/system.hpp"
#include "class/thread.hpp"
#include "class/weakref.hpp"

#include <string>
#include <sstream>

namespace rubinius {
  void Module::bootstrap(STATE) {
    GO(module).set(Class::bootstrap_class(state, G(object), ModuleType));

    // Fixup Class's superclass to be Module
    G(klass)->superclass(state, G(module));
  }

  void Module::bootstrap(STATE, Module* mod, Module* under, const char* name) {
    mod->constant_table(state, ConstantTable::create(state));
    mod->method_table(state, MethodTable::create(state));

    Symbol* sym = state->symbol(name);
    mod->set_name(state, sym, under);
    under->set_const(state, sym, mod);
  }

  void Module::bootstrap_methods(STATE) {
    System::attach_primitive(state,
                             G(module), false,
                             state->symbol("const_set"),
                             state->symbol("module_const_set"));
  }

  Module* Module::create(STATE) {
    return Module::allocate(state, G(module));
  }

  Module* Module::allocate(STATE, Object* self) {
    return state->memory()->new_module<Module>(state, as<Class>(self));
  }

  void Module::bootstrap_initialize(STATE, Module* mod, Class* super) {
    mod->method_table(nil<MethodTable>());
    mod->module_name(nil<Symbol>());
    mod->constant_table(nil<ConstantTable>());
    mod->superclass(super);
    mod->origin(mod);
    mod->seen_ivars(nil<Array>());
    mod->mirror(nil<Class>());
  }

  void Module::initialize(STATE, Module* mod) {
    mod->method_table(state, MethodTable::create(state));
    mod->module_name(nil<Symbol>());
    mod->constant_table(state, ConstantTable::create(state));
    mod->superclass(nil<Module>());
    mod->origin(mod);
    mod->seen_ivars(nil<Array>());
    mod->mirror(nil<Class>());
  }

  void Module::initialize(STATE, Module* mod, const char* name) {
    Module::initialize(state, mod, G(object), name);
  }

  void Module::initialize(STATE, Module* mod, Module* under, const char* name) {
    Module::initialize(state, mod, under, state->symbol(name));
  }

  void Module::initialize(STATE, Module* mod, Module* under, Symbol* name) {
    Module::initialize(state, mod);
    under->set_const(state, name, mod);
    mod->set_name(state, name, under);
  }

  void Module::set_name(STATE, Symbol* name, Module* under) {
    if(!module_name()->nil_p()) return;

    if(under == G(object)) {
      module_name(state, name);
    } else {
      Symbol* under_name = under->module_name();

      if(!under_name->nil_p()) {
        std::ostringstream path_name;
        path_name << under_name->cpp_str(state) << "::" << name->cpp_str(state);
        module_name(state, state->symbol(path_name.str()));

        ConstantTable::iterator i(constant_table());

        while(i.advance()) {
          if(Module* m = try_as<Module>(i.constant())) {
            if(m->module_name()->nil_p()) {
              m->set_name(state, i.name(), this);
            }
          }
        }
      }
    }
  }

  void Module::set_name(STATE, std::string name, Module* under) {
    set_name(state, state->symbol(name), under);
  }

  void Module::set_name(STATE, const char* name, Module* under) {
    set_name(state, state->symbol(name), under);
  }

  String* Module::get_name(STATE) {
    if(module_name()->nil_p()) {
      return nil<String>();
    } else {
      return module_name()->to_str(state);
    }
  }

  Object* Module::const_set(STATE, Symbol* name, Object* value) {
    set_const(state, name, value);
    return value;
  }

  void Module::set_const(STATE, Symbol* sym, Object* val) {
    constant_table()->store(state, sym, val, G(sym_public));
    state->memory()->inc_global_serial();
  }

  void Module::del_const(STATE, Symbol* sym) {
    constant_table()->remove(state, sym);
    state->memory()->inc_global_serial();
  }

  void Module::set_const(STATE, std::string name, Object* val) {
    set_const(state, state->symbol(name), val);
  }

  Object* Module::get_const(STATE, Symbol* sym, Symbol* min_vis, ConstantMissingReason* reason, bool check_super, bool replace_autoload) {
    Module* mod = this;
    *reason = vNonExistent;

    while(!mod->nil_p()) {
      ConstantTableBucket* bucket = mod->constant_table()->lookup(state, sym);

      if(!bucket->nil_p()) {
        if(min_vis == G(sym_public) &&
           bucket->visibility() == G(sym_private)) {
          *reason = vPrivate;
          break;
        } else {
          *reason = vFound;
          if(Autoload* autoload = try_as<Autoload>(bucket->constant())) {
            if(autoload->constant() != G(undefined)) {
              if(replace_autoload) {
                bucket->constant(state, autoload->constant());
              } else if (autoload->thread() == Thread::current(state)) {
                return autoload->constant();
              }
            }
          }
          return bucket->constant();
        }
      }

      if(!check_super) break;

      mod = mod->superclass();
    }
    return cNil;
  }

  Object* Module::get_const(STATE, Symbol* sym) {
    ConstantMissingReason reason;
    return get_const(state, sym, G(sym_private), &reason);
  }

  Object* Module::get_const(STATE, std::string sym) {
    return get_const(state, state->symbol(sym));
  }

  void Module::add_method(STATE, Symbol* name,
      String* method_id, Object* exec, LexicalScope* scope, Symbol* vis)
  {
    reset_method_cache(state, name);

    if(!vis) vis = G(sym_public);
    method_table()->store(state, name, method_id, exec, scope, Fixnum::from(0), vis);
  }

  Object* Module::reset_method_cache(STATE, Symbol* name) {
    if(!name->nil_p()) {
      Module* module = this;

      do {
        if(module != module->origin()) {
          module = module->superclass();
        }

        MethodTableBucket* entry = module->method_table()->find_entry(state, name);

        if(entry) {
          if(!entry->prediction()->nil_p()) {
            entry->prediction()->invalidate();
            entry->prediction(state, nil<Prediction>());
          }
        }

        module = module->superclass();

        if(module->nil_p()) break;
      } while(1);
    }

    return cNil;
  }

  Executable* Module::find_method(STATE, Symbol* name, Module** defined_in) {
    Module* mod = this;

    do {
      if(mod == mod->origin()) {
        MethodTableBucket* buk = mod->method_table()->find_entry(name);
        if(buk) {
          if(defined_in) *defined_in = mod;
          return buk->get_method(state);
        }
      }
      mod = mod->superclass();
    } while(!mod->nil_p());

    return NULL;
  }

  static Module* get_module_to_query(Module* mod) {
    Module* mod_to_query = 0;

    if(SingletonClass* sc = try_as<SingletonClass>(mod)) {
      mod_to_query = try_as<Module>(sc->singleton());
      if(!mod_to_query) mod_to_query = mod;
    } else if(IncludedModule* im = try_as<IncludedModule>(mod)) {
      mod_to_query = im->module();
    } else {
      mod_to_query = mod;
    }

    return mod_to_query;
  }

  Array* Module::class_variables(STATE) {
    Array* ary = Array::create(state, 2);
    Module* mod = this;
    Module* mod_to_query;

    class cvar_match : public ObjectMatcher {
    public:
      virtual bool match_p(STATE, Object* obj) {
        if(Symbol* sym = try_as<Symbol>(obj)) {
          return sym->is_cvar_p(state)->true_p();
        }

        return false;
      };
    } match;

    while(!mod->nil_p()) {
      mod_to_query = get_module_to_query(mod);

      Object* ivars = mod_to_query->ivars();

      if(CompactLookupTable* tbl = try_as<CompactLookupTable>(ivars)) {
        ary->concat(state, tbl->filtered_keys(state, match));
      } else if(LookupTable* tbl = try_as<LookupTable>(ivars)) {
        ary->concat(state, tbl->filtered_keys(state, match));
      }

      mod = mod->superclass();
    }

    return ary;
  }

  Object* Module::cvar_defined(STATE, Symbol* name) {
    if(!name->is_cvar_p(state)->true_p()) return Primitives::failure();

    Module* mod = this;

    while(!mod->nil_p()) {
      Module* mod_to_query = get_module_to_query(mod);

      if(mod_to_query->table_ivar_defined(state, name)->true_p()) return cTrue;

      mod = mod->superclass();
    }

    return cFalse;
  }

  Object* Module::cvar_get(STATE, Symbol* name) {
    if(!name->is_cvar_p(state)->true_p()) return Primitives::failure();

    Module* mod = this;

    while(!mod->nil_p()) {
      Module* mod_to_query = get_module_to_query(mod);

      if(mod_to_query->table_ivar_defined(state, name)->true_p()) {
        return mod_to_query->get_table_ivar(state, name);
      }

      mod = mod->superclass();
    }

    mod = this;
    if(SingletonClass* sc = try_as<SingletonClass>(mod)) {
      mod = as<Module>(sc->singleton());
    }

    std::ostringstream ss;
    ss << "uninitialized class variable ";
    ss << name->debug_str(state);
    ss << " in module ";
    if(mod->module_name()->nil_p()) {
      if(kind_of<Class>(mod)) {
        ss << "#<Class>";
      } else {
        ss << "#<Module>";
      }
    } else {
      ss << mod->debug_str(state);
    }

    RubyException::raise(
        Exception::make_exception(state,
          as<Class>(G(object)->get_const(state, "NameError")),
          ss.str().c_str()));

    return NULL;
  }

  Object* Module::cvar_set(STATE, Symbol* name, Object* value) {
    if(!name->is_cvar_p(state)->true_p()) return Primitives::failure();

    check_frozen(state);

    Module* mod = this;
    Module* mod_to_query;

    while(!mod->nil_p()) {
      mod_to_query = get_module_to_query(mod);

      if(mod_to_query->table_ivar_defined(state, name)->true_p()) {
        mod_to_query->set_table_ivar(state, name, value);
        return value;
      }

      mod = mod->superclass();
    }

    mod = this;
    mod_to_query = get_module_to_query(mod);

    mod_to_query->set_ivar(state, name, value);
    return value;
  }

  Object* Module::cvar_get_or_set(STATE, Symbol* name, Object* value) {
    if(!name->is_cvar_p(state)->true_p()) return Primitives::failure();

    Module* mod = this;
    Module* mod_to_query;

    while(!mod->nil_p()) {
      mod_to_query = get_module_to_query(mod);

      if(mod_to_query->table_ivar_defined(state, name)->true_p()) {
        return mod_to_query->get_table_ivar(state, name);
      }

      mod = mod->superclass();
    }

    mod = this;
    mod_to_query = get_module_to_query(mod);

    mod_to_query->set_table_ivar(state, name, value);
    return value;
  }

  Object* Module::cvar_remove(STATE, Symbol* name) {
    if(!name->is_cvar_p(state)->true_p()) return Primitives::failure();

    Module* mod = this;
    Module* mod_to_query;

    mod_to_query = get_module_to_query(mod);

    bool removed = false;
    Object* value = mod_to_query->del_table_ivar(state, name, &removed);
    if(removed) return value;

    std::ostringstream ss;
    if(SingletonClass* sc = try_as<SingletonClass>(mod)) {
      mod = as<Module>(sc->singleton());
    }

    if(CBOOL(this->cvar_defined(state, name))) {
      ss << "cannot remove ";
      ss << name->debug_str(state);
      ss << " for ";
    } else {
      ss << "uninitialized class variable ";
      ss << name->debug_str(state);
      ss << " in module ";
    }

    if(mod->module_name()->nil_p()) {
      if(kind_of<Class>(mod)) {
        ss << "#<Class>";
      } else {
        ss << "#<Module>";
      }
    } else {
      ss << mod->debug_str(state);
    }

    RubyException::raise(
        Exception::make_exception(state,
          as<Class>(G(object)->get_const(state, "NameError")),
          ss.str().c_str()));

    return NULL;
  }

  Class* Module::mirror(STATE, Object* obj) {
    Class* object_class = obj->class_object(state);
    Class* mirror = object_class->mirror();

    if(!mirror->nil_p()) return mirror;

    Class* klass = object_class;

    do {
      Symbol* name = klass->module_name();

      if(!name->nil_p()) {
        std::string class_name = name->cpp_str(state);
        size_t k = class_name.rfind("::");
        if(k != std::string::npos) {
          class_name = class_name.substr(k);
        }

        ConstantMissingReason reason = vNonExistent;
        Object* obj = G(mirror)->get_const(state, state->symbol(class_name), G(sym_private), &reason);

        if(reason == vFound) {
          if(Class* mirror_class = try_as<Class>(obj)) {
            object_class->mirror(state, mirror_class);
            return mirror_class;
          }
        }
      }

      Module* ancestor = klass->superclass();
      klass = 0;

      while(!ancestor->nil_p()) {
        if((klass = try_as<Class>(ancestor))) break;
        ancestor = ancestor->superclass();
      }
    } while(klass);

    return nil<Class>();
  }

  Object* Module::track_subclass(STATE, Module* mod) {
    return cNil;
  }

  std::string Module::debug_str(STATE) {
    Symbol* name = module_name();

    if(name->nil_p()) {
      return "<anonymous module>";
    } else {
      return name->cpp_str(state);
    }
  }

  /*
  std::string Module::name() {
    // TODO: temporary method to facilitate Profiler until passing State into
    // GC is fixed.
    ThreadState* vm = ThreadState::current();
    std::string name;

    Symbol* sym = module_name();

    if(sym->nil_p()) {
      std::ostringstream n;

      if(try_as<SingletonClass>(this)) {
        Module* super = superclass();

        while(kind_of<IncludedModule>(super)) {
          super = super->superclass();
        }

        n << "#<Class:"
          << vm->shared.symbols.lookup_cppstring(super->module_name())
          << ">";
      } else {
        n << "#<unknown class>";
      }

      name = n.str();
    } else {
      name = vm->shared.symbols.lookup_cppstring(sym);
    }

    return name;
  }
  */

  void Module::Info::show(STATE, Object* self, int level) {
    Module* mod = as<Module>(self);

    class_header(state, self);
    indent_attribute(++level, "name"); mod->module_name()->show(state, level);
    indent_attribute(level, "superclass"); class_info(state, mod->superclass(), true);
    indent_attribute(level, "constant_table"); mod->constant_table()->show(state, level);
    indent_attribute(level, "method_table"); mod->method_table()->show(state, level);
    close_body(level);
  }

  void Module::Info::mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) {
    auto_mark(state, obj, f);
  }

  IncludedModule* IncludedModule::create(STATE) {
    return state->memory()->new_module<IncludedModule>(
        state, G(rubinius), "<included module>");
  }

  IncludedModule* IncludedModule::allocate(STATE, Object* self) {
    IncludedModule* imod = IncludedModule::create(state);

    imod->klass(state, as<Class>(self));

    return imod;
  }
}
