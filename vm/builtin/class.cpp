#include "vm.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"

#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/methodtable.hpp"
#include "builtin/module.hpp"
#include "builtin/symbol.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/string.hpp"
#include "builtin/system.hpp"
#include "builtin/packed_object.hpp"
#include "builtin/array.hpp"

#include "builtin/executable.hpp"

#include <iostream>

namespace rubinius {

  void Class::bootstrap_methods(STATE) {
    System::attach_primitive(state,
                             G(rubinius), true,
                             state->symbol("open_class"),
                             state->symbol("vm_open_class"));

    System::attach_primitive(state,
                             G(rubinius), true,
                             state->symbol("open_class_under"),
                             state->symbol("vm_open_class_under"));

    System::attach_primitive(state,
                             G(rubinius), true,
                             state->symbol("open_module"),
                             state->symbol("vm_open_module"));

    System::attach_primitive(state,
                             G(rubinius), true,
                             state->symbol("open_module_under"),
                             state->symbol("vm_open_module_under"));

    System::attach_primitive(state,
                             G(rubinius), true,
                             state->symbol("open_metaclass"),
                             state->symbol("vm_open_metaclass"));

    System::attach_primitive(state,
                             G(rubinius), true,
                             state->symbol("add_defn_method"),
                             state->symbol("vm_add_method"));

    System::attach_primitive(state,
                             G(rubinius), true,
                             state->symbol("attach_method"),
                             state->symbol("vm_attach_method"));

    System::attach_primitive(state,
                             G(metaclass), false,
                             state->symbol("attach_method"),
                             state->symbol("metaclass_attach_method"));
  }

  Class* Class::create(STATE, Class* super) {
    Class* cls = state->om->new_object_enduring<Class>(G(klass));
    cls->class_id_ = state->shared.inc_class_count();
    cls->packed_size_ = 0;

    cls->name(state, (Symbol*)Qnil);
    cls->instance_type(state, super->instance_type());
    cls->set_type_info(super->type_info());
    cls->superclass(state, super);

    cls->setup(state);

    MetaClass::attach(state, cls, super->metaclass(state)); // HACK test

    return cls;
  }

  Class* Class::s_allocate(STATE) {
    Class* cls = as<Class>(state->om->new_object_enduring<Class>(G(klass)));
    cls->class_id_ = state->shared.inc_class_count();
    cls->set_packed_size(0);

    cls->setup(state);

    cls->set_type_info(state->om->type_info[ObjectType]);
    return cls;
  }

  Object* Class::allocate(STATE) {
    if(type_info_->type == PackedObject::type) {
      assert(packed_size_ > 0);
      PackedObject* obj = reinterpret_cast<PackedObject*>(
          state->om->new_object_fast(this, packed_size_, type_info_->type));

      uintptr_t body = reinterpret_cast<uintptr_t>(obj->body_as_array());
      for(size_t i = 0; i < packed_size_ - sizeof(ObjectHeader);
          i += sizeof(Object*)) {
        Object** pos = reinterpret_cast<Object**>(body + i);
        *pos = Qundef;
      }

      return obj;
    } else {
      return state->om->new_object_fast(this,
          type_info_->instance_size, type_info_->type);
    }
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

    packed_ivar_info(state, sup->packed_ivar_info());
    set_packed_size(sup->packed_size());

    return Qnil;
  }

  void Class::set_object_type(STATE, size_t type) {
    instance_type(state, Fixnum::from(type));
    type_info_ = state->om->type_info[type];
  }

  Object* Class::set_packed(STATE, Array* info) {
    // Only transition Object typed objects to Packed
    if(type_info_->type != Object::type) return Fixnum::from(1);

    // Reject methods that already have packing.
    if(packed_size_) return Fixnum::from(2);

    LookupTable* lt = LookupTable::create(state);

    size_t s = info->size();
    for(size_t i = 0; i < s; i++) {
      Symbol* sym = as<Symbol>(info->get(state, i));
      lt->store(state, sym, Fixnum::from(i));
    }

    packed_size_ = sizeof(Object) + (s * sizeof(Object*));
    packed_ivar_info(state, lt);

    set_object_type(state, PackedObject::type);

    return Qtrue;
  }

  MetaClass* MetaClass::attach(STATE, Object* obj, Object* sup) {
    MetaClass *meta;

    meta = state->om->new_object_enduring<MetaClass>(G(metaclass));
    meta->set_class_id(state->shared.inc_class_count());

    if(!sup) { sup = obj->klass(); }
    meta->attached_instance(state, obj);
    meta->setup(state);
    meta->superclass(state, (Module*)sup);
    meta->set_type_info(obj->klass()->type_info());

    meta->set_packed_size(obj->klass()->packed_size());
    meta->packed_ivar_info(state, obj->klass()->packed_ivar_info());

    obj->klass(state, meta);

    meta->name(state, state->symbol("<metaclass>"));

    /** @todo   These fields from Class are not set. Need to? --rue
    Fixnum* instance_type_;   // slot
    */

    return meta;
  }

  Object* MetaClass::attach_method(STATE, Symbol* name,
                                   CompiledMethod* method, StaticScope* scope)
  {
    method->scope(state, scope);
    method->serial(state, Fixnum::from(0));
    add_method(state, name, method);

    return method;
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
