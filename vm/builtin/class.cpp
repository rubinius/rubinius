#include "builtin/class.hpp"
#include "objectmemory.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/methodtable.hpp"
#include "builtin/module.hpp"
#include "builtin/symbol.hpp"
#include "builtin/string.hpp"

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

  OBJECT Class::allocate(STATE) {
    return state->new_object(this);
  }

  MetaClass* MetaClass::attach(STATE, OBJECT obj, OBJECT sup) {
    MetaClass *meta;

    meta = (MetaClass*)state->new_object(G(metaclass));
    if(!sup) { sup = obj->klass; }
    meta->IsMeta = TRUE;
    SET(meta, attached_instance, obj);
    meta->setup(state);
    SET(meta, superclass, sup);
    SET(obj, klass, meta);

    return meta;
  }
}
