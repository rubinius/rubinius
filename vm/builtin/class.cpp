#include "vm.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"
#include "configuration.hpp"

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
#include "builtin/exception.hpp"

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
                             state->symbol("object_metaclass"),
                             state->symbol("vm_object_metaclass"));

    System::attach_primitive(state,
                             G(rubinius), true,
                             state->symbol("add_defn_method"),
                             state->symbol("vm_add_method"));

    System::attach_primitive(state,
                             G(rubinius), true,
                             state->symbol("attach_method"),
                             state->symbol("vm_attach_method"));
  }

  Class* Class::create(STATE, Class* super) {
    Class* cls = state->om->new_object_enduring<Class>(G(klass));

    cls->init(state->shared.inc_class_count());

    cls->name(state, (Symbol*)Qnil);
    cls->instance_type(state, super->instance_type());

    if(super->type_info()->type == PackedObject::type) {
      cls->set_type_info(state->om->type_info[ObjectType]);
    } else {
      cls->set_type_info(super->type_info());
    }

    cls->superclass(state, super);

    cls->setup(state);

    MetaClass::attach(state, cls, super->metaclass(state)); // HACK test

    return cls;
  }

  Class* Class::s_allocate(STATE) {
    Class* cls = as<Class>(state->om->new_object_enduring<Class>(G(klass)));

    cls->init(state->shared.inc_class_count());
    cls->setup(state);

    cls->set_type_info(state->om->type_info[ObjectType]);
    return cls;
  }

  void Class::init(int id) {
    class_id_ = id;
    set_packed_size(0);
    building_ = true;
  }

  Object* Class::allocate(STATE) {
    if(type_info_->type == PackedObject::type) {
use_packed:
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
    } else if(!type_info_->allow_user_allocate) {
      Exception::type_error(state, "direct allocation disabled");
      return Qnil;
    } else if(!building_) {
      return state->om->new_object_fast(this,
          type_info_->instance_size, type_info_->type);
    } else {
      if(type_info_->type == Object::type) {
        if(!seen_ivars_->nil_p()) {
          if(auto_pack(state)) goto use_packed;
        }
      }

      building_ = false;

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

  Object* Class::set_superclass(STATE, Object* obj) {
    if(obj->nil_p()) {
      superclass(state, (Class*)Qnil);
      return Qnil;
    }

    Class* sup;
    if((sup = try_as<Class>(obj)) == 0) {
      return Primitives::failure();
    }

    superclass(state, sup);

    instance_type(state, sup->instance_type());
    if(sup->type_info()->type == PackedObject::type) {
      set_type_info(state->om->type_info[ObjectType]);
    } else {
      set_type_info(sup->type_info());
    }

    MetaClass::attach(state, this, sup->metaclass(state));

    return Qnil;
  }

  void Class::set_object_type(STATE, size_t type) {
    instance_type(state, Fixnum::from(type));
    type_info_ = state->om->type_info[type];
  }

  bool Class::auto_pack(STATE) {
    if(building_) {
      building_ = false;

      // If autopack is off, ignore this.
      if(!state->shared.config.gc_autopack) return false;

      // Only transition Object typed objects to Packed
      if(type_info_->type != Object::type) return false;

      // Reject methods that already have packing.
      if(packed_size_) return true;

      LookupTable* lt = LookupTable::create(state);

      Module* mod = this;

      int slot = 0;

      while(!mod->nil_p()) {
        if(Class* cls = try_as<Class>(mod)) {
          Array* info = cls->seen_ivars();

          if(!info->nil_p()) {
            for(size_t i = 0; i < info->size(); i++) {
              if(Symbol* sym = try_as<Symbol>(info->get(state, i))) {
                bool found = false;
                lt->fetch(state, sym, &found);

                if(!found) {
                  lt->store(state, sym, Fixnum::from(slot++));
                }
              }
            }
          }
        }

        mod = mod->superclass();
      }

      size_t slots = lt->entries()->to_native();

      packed_size_ = sizeof(Object) + (slots * sizeof(Object*));
      packed_ivar_info(state, lt);

      set_object_type(state, PackedObject::type);
    }

    return type_info_->type == PackedObject::type;
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

  Class* Class::real_class(STATE, Class* klass) {
    if(MetaClass* mc = try_as<MetaClass>(klass)) {
      return mc->true_superclass(state);
    } else {
      return klass;
    }
  }

  MetaClass* MetaClass::attach(STATE, Object* obj, Class* sup) {
    MetaClass *meta;
    meta = state->om->new_object_enduring<MetaClass>(G(klass));
    meta->init(state->shared.inc_class_count());

    meta->attached_instance(state, obj);
    meta->setup(state);

    meta->set_type_info(obj->klass()->type_info());

    meta->set_packed_size(obj->klass()->packed_size());
    meta->packed_ivar_info(state, obj->klass()->packed_ivar_info());

    /* The superclass hierarchy for metaclasses lives in parallel to that of classes.
     * This code ensures that the superclasses of metaclasses are also metaclasses.
     */
    if(MetaClass* already_meta = try_as<MetaClass>(obj)) {
      /* If we are attaching a metaclass to something that is already a MetaClass,
       * make the metaclass's superclass be the attachee's superclass.
       * klass and superclass are both metaclasses in this case.
       */
      meta->klass(state, meta);
      meta->superclass(state, already_meta->true_superclass(state)->metaclass(state));
    } else {
      /* If we are attaching to anything but a MetaClass, the new
       * metaclass's class is the same as its superclass.
       * This is where the superclass chains for meta/non-meta classes diverge.
       * If no superclass argument was provided, we use the klass we are replacing.
       */
      if(!sup) { sup = obj->klass(); }
      /* Tell the new MetaClass about the attachee's existing hierarchy */
      Class* meta_klass = Class::real_class(state, sup)->klass();
      meta->klass(state, meta_klass);
      meta->superclass(state, sup);
    }

    /* Finally, attach the new MetaClass */
    obj->klass(state, meta);

    return meta;
  }

  Object* Class::get_metaclass_attached(STATE) {
    if(MetaClass* mc = try_as<MetaClass>(this)) {
      return mc->attached_instance();
    }

    return Qnil;
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

    std::cout << "#<MetaClass:" << self->class_object(state)->name()->c_str(state) <<
      " " << name << ":" << (void*)self << ">" << std::endl;
  }
}
