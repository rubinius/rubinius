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

#include "builtin/executable.hpp"

#include <iostream>

namespace rubinius {

  static void hookup_prim(STATE, Symbol* meth, Symbol* prim) {
    LookupTable* tbl = G(rubinius)->metaclass(state)->method_table();

    Executable* oc = Executable::allocate(state, Qnil);
    oc->primitive(state, prim);
    assert(oc->resolve_primitive(state));

    tbl->store(state, meth, oc);
  }

  void Class::bootstrap_methods(STATE) {
    hookup_prim(state,
                state->symbol("open_class"),
                state->symbol("vm_open_class"));

    hookup_prim(state,
                state->symbol("open_class_under"),
                state->symbol("vm_open_class_under"));

    hookup_prim(state,
                state->symbol("open_module"),
                state->symbol("vm_open_module"));

    hookup_prim(state,
                state->symbol("open_module_under"),
                state->symbol("vm_open_module_under"));

    hookup_prim(state,
                state->symbol("open_metaclass"),
                state->symbol("vm_open_metaclass"));

    hookup_prim(state,
                state->symbol("add_defn_method"),
                state->symbol("vm_add_method"));

    hookup_prim(state,
                state->symbol("attach_method"),
                state->symbol("vm_attach_method"));
  }

  Class* Class::create(STATE, Class* super) {
    Class* cls = state->new_object<Class>(G(klass));
    cls->class_id_ = state->shared.inc_class_count();

    cls->name(state, (Symbol*)Qnil);
    cls->instance_type(state, super->instance_type());
    cls->set_type_info(super->type_info());
    cls->superclass(state, super);

    cls->setup(state);

    MetaClass::attach(state, cls, super->metaclass(state)); // HACK test

    return cls;
  }

  Class* Class::s_allocate(STATE) {
    Class* cls = as<Class>(state->new_object<Class>(G(klass)));
    cls->class_id_ = state->shared.inc_class_count();

    cls->set_type_info(state->om->type_info[ObjectType]);
    return cls;
  }

  Object* Class::allocate(STATE) {
    TypeInfo* ti = state->find_type(instance_type_->to_native());
    return state->new_object_from_type(this, ti);
  }

  Class* Class::true_superclass(STATE) {
    Module* super = superclass();

    while(kind_of<IncludedModule>(super)) {
      super = super->superclass();
    }

    return as<Class>(super);
  }

  Object* Class::set_superclass(STATE, Class* sup) {
    superclass(state, sup);
    instance_type(state, sup->instance_type());
    set_type_info(sup->type_info());

    return Qnil;
  }

  void Class::set_object_type(STATE, size_t type) {
    instance_type(state, Fixnum::from(type));
    type_info_ = state->om->type_info[type];
  }

  MetaClass* MetaClass::attach(STATE, Object* obj, Object* sup) {
    MetaClass *meta;

    meta = state->new_object<MetaClass>(G(metaclass));
    meta->set_class_id(state->shared.inc_class_count());

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
