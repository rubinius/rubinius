#include "vm.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"
#include "configuration.hpp"
#include "on_stack.hpp"

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

  Class* Class::create(STATE, Class* super) {
    Class* cls = state->om->new_object_enduring<Class>(state, G(klass));

    cls->init(state->shared.inc_class_count(state));

    cls->name(state, nil<Symbol>());
    cls->instance_type(state, super->instance_type());

    if(super->type_info()->type == PackedObject::type) {
      cls->set_type_info(state->om->type_info[ObjectType]);
    } else {
      cls->set_type_info(super->type_info());
    }

    cls->superclass(state, super);

    cls->setup(state);

    SingletonClass::attach(state, cls, super->singleton_class(state)); // HACK test

    return cls;
  }

  Class* Class::s_allocate(STATE) {
    Class* cls = as<Class>(state->om->new_object_enduring<Class>(state, G(klass)));

    cls->init(state->shared.inc_class_count(state));
    cls->setup(state);

    cls->set_type_info(state->om->type_info[ObjectType]);
    return cls;
  }

  void Class::init(int id) {
    class_id_ = id;
    set_packed_size(0);
  }

  Object* Class::allocate(STATE, CallFrame* calling_environment) {
    if(type_info_->type == PackedObject::type) {
use_packed:
      assert(packed_size_ > 0);

      // Pull the size out into a local to deal with this moving later on.
      uint32_t size = packed_size_;

      PackedObject* obj = state->local_slab().allocate(size).as<PackedObject>();

      if(likely(obj)) {
        obj->init_header(this, YoungObjectZone, PackedObject::type);
      } else {
        if(state->shared.om->refill_slab(state, state->local_slab())) {
          obj = state->local_slab().allocate(size).as<PackedObject>();

          if(likely(obj)) {
            obj->init_header(this, YoungObjectZone, PackedObject::type);
          } else {
            obj = reinterpret_cast<PackedObject*>(
                state->om->new_object_fast(state, this, size, PackedObject::type));
          }
        } else {
          state->shared.om->collect_young_now = true;

          Class* self = this;

          OnStack<1> os(state, self);

          state->collect_maybe(calling_environment);

          // Don't use 'this' after here! it's been moved! use 'self'!

          obj = state->local_slab().allocate(size).as<PackedObject>();

          if(likely(obj)) {
            obj->init_header(self, YoungObjectZone, PackedObject::type);
          } else {
            obj = reinterpret_cast<PackedObject*>(
                state->om->new_object_fast(state, self, size, PackedObject::type));
          }
        }
      }

      // Don't use 'this' !!! The above code might have GC'd

      uintptr_t body = reinterpret_cast<uintptr_t>(obj->body_as_array());
      for(size_t i = 0; i < size - sizeof(ObjectHeader);
          i += sizeof(Object*)) {
        Object** pos = reinterpret_cast<Object**>(body + i);
        *pos = Qundef;
      }

      return obj;
    } else if(!type_info_->allow_user_allocate || kind_of<SingletonClass>(this)) {
      Exception::type_error(state, "direct allocation disabled");
      return Qnil;
    } else if(type_info_->type == Object::type) {
      // transition all normal object classes to PackedObject
      auto_pack(state);
      goto use_packed;
    }

    // type_info_->type is neither PackedObject nor Object, so use the
    // generic path.
    return state->new_object_typed(this,
        type_info_->instance_size, type_info_->type);
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
      superclass(state, nil<Class>());
      return Qnil;
    }

    Class* sup;
    if((sup = try_as<Class>(obj)) == 0) {
      return Primitives::failure();
    }

    if(try_as<SingletonClass>(sup)) {
      Exception::type_error(state, "cannot inherit from a singleton class");
    }

    superclass(state, sup);

    instance_type(state, sup->instance_type());
    if(sup->type_info()->type == PackedObject::type) {
      set_type_info(state->om->type_info[ObjectType]);
    } else {
      set_type_info(sup->type_info());
    }

    SingletonClass::attach(state, this, sup->singleton_class(state));

    return Qnil;
  }

  void Class::set_object_type(STATE, size_t type) {
    instance_type(state, Fixnum::from(type));
    type_info_ = state->om->type_info[type];
  }

  /* Look at this class and it's superclass contents (which includes
   * included modules) and calculate out how to allocate the slots.
   *
   * This locks the class so that construction is serialized.
   */
  void Class::auto_pack(STATE) {
    hard_lock(state);

    // If another thread did this work while we were waiting on the lock,
    // don't redo it.
    if(type_info_->type == PackedObject::type) return;

    size_t slots = 0;

    LookupTable* lt = LookupTable::create(state);

    // If autopacking is enabled, figure out how many slots to use.
    if(state->shared.config.gc_autopack) {
      Module* mod = this;

      int slot = 0;

      while(!mod->nil_p()) {
        Array* info = 0;

        if(Class* cls = try_as<Class>(mod)) {
          info = cls->seen_ivars();
        } else if(IncludedModule* im = try_as<IncludedModule>(mod)) {
          info = im->module()->seen_ivars();
        }

        if(info && !info->nil_p()) {
          for(size_t i = 0; i < info->size(); i++) {
            if(Symbol* sym = try_as<Symbol>(info->get(state, i))) {
              bool found = false;
              lt->fetch(state, sym, &found);

              if(!found) {
                lt->store(state, sym, Fixnum::from(slot++));
              }
            }

            // Limit the number of packed ivars to 25.
            if(slot > 25) break;
          }
        }

        mod = mod->superclass();
      }
      slots = lt->entries()->to_native();
    }

    packed_size_ = sizeof(Object) + (slots * sizeof(Object*));
    packed_ivar_info(state, lt);

    set_object_type(state, PackedObject::type);

    hard_unlock(state);
  }

  Class* Class::real_class(STATE, Class* klass) {
    if(SingletonClass* sc = try_as<SingletonClass>(klass)) {
      return sc->true_superclass(state);
    } else {
      return klass;
    }
  }

  SingletonClass* SingletonClass::attach(STATE, Object* obj, Class* sup) {
    SingletonClass *sc;
    sc = state->om->new_object_enduring<SingletonClass>(state, G(klass));
    sc->init(state->shared.inc_class_count(state));

    sc->attached_instance(state, obj);
    sc->setup(state);

    if(kind_of<PackedObject>(obj)) {
      sc->set_type_info(state->om->type_info[Object::type]);
    } else {
      sc->set_type_info(obj->klass()->type_info());
    }

    sc->set_packed_size(obj->klass()->packed_size());
    sc->packed_ivar_info(state, obj->klass()->packed_ivar_info());

    /* The superclass hierarchy for singleton classes lives in parallel to
     * that of classes.  This code ensures that the superclasses of singleton
     * classes are also singleton classes.
     */
    if(SingletonClass* already_sc = try_as<SingletonClass>(obj)) {
      /* If we are attaching a singleton class to something that is already a
       * SingletonClass, make the singleton class's superclass be the attachee's
       * superclass.  klass and superclass are both singleton classes in this
       * case.
       */
      sc->klass(state, sc);
      sc->superclass(state, already_sc->true_superclass(state)->singleton_class(state));
    } else {
      /* If we are attaching to anything but a SingletonClass, the new
       * singleton class's class is the same as its superclass.  This is where
       * the superclass chains for singleton and non-singleton classes
       * diverge.  If no superclass argument was provided, we use the klass we
       * are replacing.
       */
      if(!sup) { sup = obj->klass(); }
      /* Tell the new SingletonClass about the attachee's existing hierarchy */
      Class* super_klass = Class::real_class(state, sup)->klass();
      sc->klass(state, super_klass);
      sc->superclass(state, sup);
    }

    /* Finally, attach the new SingletonClass */
    obj->klass(state, sc);

    return sc;
  }

  void SingletonClass::Info::show(STATE, Object* self, int level) {
    SingletonClass* cls = as<SingletonClass>(self);
    Module* mod = try_as<Module>(cls->attached_instance());

    std::string name;

    if(mod) {
      name = mod->name()->nil_p() ? "<anonymous>" : mod->name()->debug_str(state);
    } else {
      name = "<some object>";
    }

    std::cout << "#<SingletonClass:" << self->class_object(state)->name()->debug_str(state) <<
      " " << name << ":" << (void*)self << ">" << std::endl;
  }
}
