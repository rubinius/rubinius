#include "vm.hpp"
#include "builtin_class.hpp"
#include "objectmemory.hpp"

namespace rubinius {

  Class* Class::create(STATE, Class* super) {
    Class* cls = (Class*)state->new_object(G(klass));

    SET(cls, name, Qnil);
    SET(cls, instance_fields, super->instance_fields);
    SET(cls, has_ivars, super->has_ivars);
    SET(cls, instance_type, super->instance_type);
    SET(cls, superclass, super);

    cls->setup(state);

    return cls;
  }

  Module* Module::create(STATE) {
    Module* mod = (Module*)state->new_object(G(module));

    SET(mod, name, Qnil);
    SET(mod, superclass, Qnil);

    mod->setup(state);

    return mod;
  }

  void Module::setup(STATE) {
    SET(this, constants, LookupTable::create(state));
    SET(this, method_table, MethodTable::create(state));
  }

  void Module::setup(STATE, char* str, Module* under) {
    setup(state, state->symbol(str), under);
  }

  void Module::setup(STATE, SYMBOL name, Module* under) {
    if(!under) under = G(object);

    SET(this, constants, LookupTable::create(state));
    SET(this, method_table, MethodTable::create(state));
    SET(this, name, name);
    under->set_const(state, name, this);
  }

  void Module::set_name(STATE, Module* under, SYMBOL name) {
    String* cur = under->name->to_str(state);
    String* str_name = cur->add(state, "::")->append(state,
        name->to_str(state));

    SET(this, name, str_name->to_sym(state));
  }

  void Module::set_const(STATE, OBJECT sym, OBJECT val) {
    constants->store(state, sym, val);
  }

  void Module::set_const(STATE, char* name, OBJECT val) {
    constants->store(state, state->symbol(name), val);
  }

  OBJECT Module::get_const(STATE, SYMBOL sym) {
    return constants->fetch(state, sym);
  }

  OBJECT Module::get_const(STATE, SYMBOL sym, bool* found) {
    return constants->fetch(state, sym, found);
  }

  OBJECT Module::get_const(STATE, char* sym) {
    return get_const(state, state->symbol(sym));
  }

  MetaClass* MetaClass::attach(STATE, OBJECT obj, OBJECT sup) {
    MetaClass *meta;

    meta = (MetaClass*)state->om->new_object(G(metaclass),
                                             MetaClass::fields);
    if(!sup) { sup = obj->klass; }
    meta->IsMeta = TRUE;
    SET(meta, attached_instance, obj);
    meta->setup(state);
    SET(meta, superclass, sup);
    state->om->set_class(obj, meta);

    return meta;
  }
}
