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

#include "global_cache.hpp"

namespace rubinius {

  void Module::bootstrap_methods(STATE) {
    System::attach_primitive(state,
                             G(module), false,
                             state->symbol("const_set"),
                             state->symbol("module_const_set"));
  }

  Module* Module::create(STATE) {
    Module* mod = state->om->new_object_enduring<Module>(G(module));

    mod->name(state, nil<Symbol>());
    mod->superclass(state, nil<Module>());

    mod->setup(state);

    return mod;
  }

  Module* Module::allocate(STATE, Object* self) {
    Module* module = Module::create(state);

    module->klass(state, (Class*)self);

    return module;
  }

  void Module::setup(STATE) {
    constant_table(state, LookupTable::create(state));
    method_table(state, MethodTable::create(state));
  }

  void Module::setup(STATE, const char* str, Module* under) {
    setup(state, state->symbol(str), under);

    if(under && under != G(object)) {
      std::ostringstream ss;
      ss << under->name()->c_str(state) << "::" << str;
      this->name(state, state->symbol(ss.str().c_str()));
    }
  }

  void Module::setup(STATE, Symbol* name, Module* under) {
    if(!under) under = G(object);

    constant_table(state, LookupTable::create(state));
    this->method_table(state, MethodTable::create(state));
    this->name(state, name);
    under->set_const(state, name, this);
  }

  Object* Module::case_compare(STATE, Object* obj) {
    return obj->kind_of_p(state, this) ? Qtrue : Qfalse;
  }

  void Module::set_name(STATE, Module* under, Symbol* name) {
    if(under == G(object)) {
      this->name(state, name);
    } else {
      String* cur = under->name()->to_str(state);
      String* str_name = cur->add(state, "::")->append(state,
          name->to_str(state));

      this->name(state, str_name->to_sym(state));
    }
  }

  Object* Module::const_set(STATE, Object* name, Object* value) {
    set_const(state, name, value);
    return value;
  }

  void Module::set_const(STATE, Object* sym, Object* val) {
    constants()->store(state, sym, val);
    state->shared.inc_global_serial();
  }

  void Module::del_const(STATE, Symbol* sym) {
    constants()->remove(state, sym);
    state->shared.inc_global_serial();
  }

  void Module::set_const(STATE, const char* name, Object* val) {
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

    return Qnil;
  }

  Object* Module::get_const(STATE, Symbol* sym) {
    bool found;
    return get_const(state, sym, &found);
  }

  Object* Module::get_const(STATE, const char* sym) {
    return get_const(state, state->symbol(sym));
  }

  void Module::add_method(STATE, Symbol* name, Executable* exec, Symbol* vis) {
    if(!vis) vis = G(sym_public);
    method_table_->store(state, name, exec, vis);
    state->global_cache()->clear(this, name);
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

      if(mod_to_query->table_ivar_defined(state, name)->true_p()) return Qtrue;

      mod = mod->superclass();
    }

    return Qfalse;
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

    std::stringstream ss;
    ss << "uninitialized class variable ";
    ss << name->c_str(state);
    ss << " in module ";
    if(mod->name()->nil_p()) {
      if(kind_of<Class>(mod)) {
        ss << "#<Class>";
      } else {
        ss << "#<Module>";
      }
    } else {
      ss << mod->name()->c_str(state);
    }

    RubyException::raise(
        Exception::make_exception(state,
          as<Class>(G(object)->get_const(state, "NameError")),
          ss.str().c_str()));

    return NULL;
  }

  Object* Module::cvar_set(STATE, Symbol* name, Object* value) {
    if(!name->is_cvar_p(state)->true_p()) return Primitives::failure();

    if(RTEST(frozen_p(state))) {
      Exception::type_error(state, "unable to change frozen object");
    }

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

    std::stringstream ss;
    mod = this;
    if(SingletonClass* sc = try_as<SingletonClass>(mod)) {
      mod = as<Module>(sc->attached_instance());
    }

    if (this->cvar_defined(state, name) == Qtrue) {
      ss << "cannot remove ";
      ss << name->c_str(state);
      ss << " for ";
    } else {
      ss << "uninitialized class variable ";
      ss << name->c_str(state);
      ss << " in module ";
    }

    if(mod->name()->nil_p()) {
      if(kind_of<Class>(mod)) {
        ss << "#<Class>";
      } else {
        ss << "#<Module>";
      }
    } else {
      ss << mod->name()->c_str(state);
    }

    RubyException::raise(
        Exception::make_exception(state,
          as<Class>(G(object)->get_const(state, "NameError")),
          ss.str().c_str()));

    return NULL;
  }

  void Module::Info::show(STATE, Object* self, int level) {
    Module* mod = as<Module>(self);

    class_header(state, self);
    indent_attribute(++level, "name"); mod->name()->show(state, level);
    indent_attribute(level, "superclass"); class_info(state, mod->superclass(), true);
    indent_attribute(level, "constants"); mod->constants()->show(state, level);
    indent_attribute(level, "method_table"); mod->method_table()->show(state, level);
    close_body(level);
  }

  IncludedModule* IncludedModule::create(STATE) {
    IncludedModule* imod;
    imod = state->om->new_object_enduring<IncludedModule>(G(included_module));

    imod->name(state, state->symbol("<included module>"));
    imod->superclass(state, nil<Module>());

    return imod;
  }

  IncludedModule* IncludedModule::allocate(STATE, Object* self) {
    IncludedModule* imod = IncludedModule::create(state);

    imod->klass(state, (Class*)self);

    return imod;
  }

}
