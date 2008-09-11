#include "vm.hpp"
#include "objectmemory.hpp"

#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/methodtable.hpp"
#include "builtin/module.hpp"
#include "builtin/symbol.hpp"
#include "builtin/string.hpp"

#include <iostream>

namespace rubinius {

  template <>
    bool kind_of<Class>(OBJECT obj) {
      return obj->obj_type == ClassType ||
        obj->obj_type == MetaclassType ||
        obj->obj_type == IncModType;
    }

  template <>
    bool kind_of<Module>(OBJECT obj) {
      return obj->reference_p() &&
        (obj->obj_type == Module::type ||
         obj->obj_type == Class::type ||
         obj->obj_type == MetaClass::type ||
         obj->obj_type == IncludedModule::type);
    }


  Class* Class::create(STATE, Class* super) {
    Class* cls = (Class*)state->new_object(G(klass));

    cls->name(state, (SYMBOL)Qnil);
    cls->instance_fields(state, super->instance_fields());
    cls->instance_type(state, super->instance_type());
    cls->superclass(state, super);

    cls->setup(state);

    MetaClass::attach(state, cls, super->metaclass(state)); // HACK test

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
    meta->attached_instance(state, obj);
    meta->setup(state);
    meta->superclass(state, (Module*)sup);
    SET(obj, klass, meta);

    return meta;
  }

  void MetaClass::Info::show(STATE, OBJECT self, int level) {
    MetaClass* cls = as<MetaClass>(self);
    Module* mod = as<Module>(cls->attached_instance());

    const char* name = mod->name()->nil_p() ? "<anonymous>" : mod->name()->c_str(state);
    std::cout << "#<" << self->class_object(state)->name()->c_str(state) <<
      " " << name << ":" << (void*)self << ">" << std::endl;
  }
}
