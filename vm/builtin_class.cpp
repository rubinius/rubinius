#include "vm.hpp"
#include "builtin_class.hpp"
#include "objectmemory.hpp"

namespace rubinius {
  void Module::setup(STATE) {
    SET(this, constants, LookupTable::create(state));
    SET(this, method_table, MethodTable::create(state));
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
