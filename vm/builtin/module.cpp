#include "vm.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"

#include "builtin/class.hpp"
#include "builtin/module.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/methodtable.hpp"
#include "builtin/symbol.hpp"
#include "builtin/string.hpp"
#include "builtin/system.hpp"
#include "builtin/array.hpp"
#include "builtin/compactlookuptable.hpp"

#include "configuration.hpp"
#include "global_cache.hpp"

#include "on_stack.hpp"

namespace rubinius {

  void Module::bootstrap_methods(STATE) {
    GCTokenImpl gct;
    System::attach_primitive(state, gct,
                             G(module), false,
                             state->symbol("const_set"),
                             state->symbol("module_const_set"));
  }

  Module* Module::create(STATE) {
    Module* mod = state->memory()->new_object_enduring<Module>(state, G(module));

    mod->module_name(state, nil<Symbol>());
    mod->superclass(state, nil<Module>());

    mod->setup(state);

    return mod;
  }

  Module* Module::allocate(STATE, Object* self) {
    Module* module = Module::create(state);

    module->klass(state, as<Class>(self));

    return module;
  }

  void Module::setup(STATE) {
    constant_table(state, LookupTable::create(state));
    method_table(state, MethodTable::create(state));
  }

  void Module::setup(STATE, std::string name, Module* under) {
    setup(state);

    if(!under) under = G(object);
    under->set_const(state, name, this);
    set_name(state, name, under);
  }

  Object* Module::case_compare(STATE, Object* obj) {
    return obj->kind_of_p(state, this) ? cTrue : cFalse;
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

        LookupTable::iterator i(constants());

        while(i.advance()) {
          if(Module* m = try_as<Module>(i.value())) {
            if(m->module_name()->nil_p()) {
              m->set_name(state, as<Symbol>(i.key()), this);
            }
          }
        }
      }
    }
  }

  void Module::set_name(STATE, std::string name, Module* under) {
    set_name(state, state->symbol(name), under);
  }

  String* Module::get_name(STATE) {
    if(module_name()->nil_p()) {
      if(LANGUAGE_18_ENABLED(state)) {
        return String::create(state, "");
      } else {
        return nil<String>();
      }
    } else {
      return module_name()->to_str(state);
    }
  }

  Object* Module::const_set(STATE, Object* name, Object* value) {
    set_const(state, name, value);
    return value;
  }

  void Module::set_const(STATE, Object* sym, Object* val) {
    constants()->store(state, sym, val);
    state->shared().inc_global_serial(state);
  }

  void Module::del_const(STATE, Symbol* sym) {
    constants()->remove(state, sym);
    state->shared().inc_global_serial(state);
  }

  void Module::set_const(STATE, std::string name, Object* val) {
    set_const(state, state->symbol(name), val);
  }

  Object* Module::get_const(STATE, Symbol* sym, bool* found, bool check_super) {
    Module* mod = this;

    while(!mod->nil_p()) {
      Object* obj = mod->constants()->fetch(state, sym, found);

      if(*found) return obj;

      if(!check_super) break;

      mod = mod->superclass();
    }

    return cNil;
  }

  Object* Module::get_const(STATE, Symbol* sym) {
    bool found;
    return get_const(state, sym, &found);
  }

  Object* Module::get_const(STATE, std::string sym) {
    return get_const(state, state->symbol(sym));
  }

  void Module::add_method(STATE, GCToken gct, Symbol* name, Executable* exec,
                          Symbol* vis)
  {
    Module* self = this;
    OnStack<2> os(state, self, exec);

    if(!vis) vis = G(sym_public);
    method_table_->store(state, gct, name, exec, vis);
    state->vm()->global_cache()->clear(state, self, name);
  }

  Executable* Module::find_method(Symbol* name, Module** defined_in) {
    MethodTableBucket* buk;
    Module* mod = this;

    do {
      buk = mod->method_table()->find_entry(name);
      if(buk) {
        if(defined_in) *defined_in = mod;
        return buk->method();
      }
      mod = mod->superclass();
    } while(!mod->nil_p());

    return NULL;
  }

  static Module* get_module_to_query(Module* mod) {
    Module* mod_to_query = 0;

    if(SingletonClass* sc = try_as<SingletonClass>(mod)) {
      mod_to_query = try_as<Module>(sc->attached_instance());
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
    Module* mod_to_query;

    while(!mod->nil_p()) {
      mod_to_query = get_module_to_query(mod);

      if(mod_to_query->table_ivar_defined(state, name)->true_p()) return cTrue;

      mod = mod->superclass();
    }

    return cFalse;
  }

  Object* Module::cvar_get(STATE, Symbol* name) {
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
    if(SingletonClass* sc = try_as<SingletonClass>(mod)) {
      mod = as<Module>(sc->attached_instance());
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
      mod = as<Module>(sc->attached_instance());
    }

    if (this->cvar_defined(state, name) == cTrue) {
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

  std::string Module::debug_str(STATE) {
    Symbol* name = module_name();

    if(name->nil_p()) {
      return "<anonymous module>";
    } else {
      return name->cpp_str(state);
    }
  }

  void Module::Info::show(STATE, Object* self, int level) {
    Module* mod = as<Module>(self);

    class_header(state, self);
    indent_attribute(++level, "name"); mod->module_name()->show(state, level);
    indent_attribute(level, "superclass"); class_info(state, mod->superclass(), true);
    indent_attribute(level, "constants"); mod->constants()->show(state, level);
    indent_attribute(level, "method_table"); mod->method_table()->show(state, level);
    close_body(level);
  }

  IncludedModule* IncludedModule::create(STATE) {
    IncludedModule* imod;
    imod = state->memory()->new_object_enduring<IncludedModule>(state, G(included_module));

    imod->module_name(state, state->symbol("<included module>"));
    imod->superclass(state, nil<Module>());

    return imod;
  }

  IncludedModule* IncludedModule::allocate(STATE, Object* self) {
    IncludedModule* imod = IncludedModule::create(state);

    imod->klass(state, as<Class>(self));

    return imod;
  }

}
