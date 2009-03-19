#include "vm.hpp"
#include "vm/object_utils.hpp"
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

  Class* Class::create(STATE, Class* super) {
    Class* cls = state->new_object<Class>(G(klass));

    cls->name(state, (Symbol*)Qnil);
    cls->instance_type(state, super->instance_type());
    cls->set_type_info(super->type_info());
    cls->superclass(state, super);

    cls->setup(state);

    MetaClass::attach(state, cls, super->metaclass(state)); // HACK test

    return cls;
  }

  Class* Class::s_allocate(STATE) {
    return as<Class>(state->new_object<Class>(G(klass)));
  }

  Object* Class::allocate(STATE) {
    TypeInfo* ti = state->find_type(instance_type_->to_native());
    return state->new_object_from_type(this, ti);
  }

  Class* Class::direct_superclass(STATE) {
    Module* super = superclass();

    while(kind_of<IncludedModule>(super)) {
      super = super->superclass();
    }

    return as<Class>(super);
  }

  MetaClass* MetaClass::attach(STATE, Object* obj, Object* sup) {
    MetaClass *meta;

    meta = state->new_object<MetaClass>(G(metaclass));
    if(!sup) { sup = obj->klass(); }
    meta->attached_instance(state, obj);
    meta->setup(state);
    meta->superclass(state, (Module*)sup);
    meta->set_type_info(obj->klass()->type_info());
    obj->klass(state, meta);

    meta->name(state, state->symbol("<metaclass>"));

    /** @todo   These fields from Class are not set. Need to? --rue
    Fixnum* instance_type_;   // slot
    */

    return meta;
  }

  void MetaClass::Info::show(STATE, Object* self, int level) {
    MetaClass* cls = as<MetaClass>(self);
    Module* mod = try_as<Module>(cls->attached_instance());

    const char* name;

    if(mod) {
      name = mod->name()->nil_p() ? "<anonymous>" : mod->name()->c_str(state);
    } else {
      name = "<some object>";
    }

    std::cout << "#<" << self->class_object(state)->name()->c_str(state) <<
      " " << name << ":" << (void*)self << ">" << std::endl;
  }
}
