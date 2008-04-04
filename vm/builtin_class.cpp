#include "vm.hpp"
#include "builtin_class.hpp"
#include "objectmemory.hpp"

namespace rubinius {
  void Module::setup(STATE) {
    SET(this, constants, LookupTable::create(state));
    SET(this, method_table, MethodTable::create(state));
  }

  void Module::setup(STATE, char* str, Module* under) {
    if(!under) under = state->globals.object;

    SET(this, constants, LookupTable::create(state));
    SET(this, method_table, MethodTable::create(state));
    SET(this, name, state->symbol(str));
    under->set_const(state, name, this);
  }

  void Module::set_const(STATE, OBJECT sym, OBJECT val) {
    constants->store(state, sym, val);
  }

  OBJECT Module::get_const(STATE, OBJECT sym) {
    return constants->fetch(state, sym);
  }

  OBJECT Module::get_const(STATE, char* sym) {
    return get_const(state, state->symbol(sym));

  }

  OBJECT MetaClass::attach(STATE, OBJECT obj, OBJECT sup) {
    MetaClass *meta;

    meta = (MetaClass*)state->om->new_object(state->globals.metaclass,
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
