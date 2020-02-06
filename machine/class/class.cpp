#include "configuration.hpp"
#include "object_utils.hpp"
#include "memory.hpp"
#include "primitives.hpp"
#include "on_stack.hpp"

#include "class/array.hpp"
#include "class/class.hpp"
#include "class/constant_table.hpp"
#include "class/exception.hpp"
#include "class/fixnum.hpp"
#include "class/lookup_table.hpp"
#include "class/method_table.hpp"
#include "class/module.hpp"
#include "class/packed_object.hpp"
#include "class/symbol.hpp"

#include <atomic>
#include <sstream>

namespace rubinius {
  void Class::bootstrap(STATE) {
    GO(klass).set(Class::bootstrap_class(state, nil<Class>(), ClassType));
    G(klass)->klass(state, G(klass));
  }

  Class* Class::bootstrap_class(STATE, Class* super, object_type type) {
    Class* klass = static_cast<Class*>(state->memory()->new_object(
          state, sizeof(Class), ClassType));
    Class::bootstrap_initialize(state, klass, super, type);

    return klass;
  }

  void Class::bootstrap_initialize(STATE, Class* klass, Class* super, object_type type) {
    Module::bootstrap_initialize(state, klass, super);

    klass->klass(state, G(klass));
    klass->ivars(cNil);

    // The type of object this class makes (eg Tuple).
    klass->instance_type(Fixnum::from(type));
    klass->type_info(state->memory()->type_info[type]);

    klass->superclass(state, super);

    Class::initialize_data(state, klass);
  }

  void Class::initialize_data(STATE, Class* klass) {
    klass->class_data(state->memory()->inc_class_count());
    klass->packed_size(0);

    klass->packed_ivar_info(nil<LookupTable>());
  }

  void Class::initialize_type(STATE, Class* klass, Class* super) {
    klass->superclass(state, super);
    klass->instance_type(state, super->instance_type());

    if(super->type_info()->type == PackedObjectType) {
      klass->type_info(state->memory()->type_info[ObjectType]);
    } else {
      klass->type_info(super->type_info());
    }

    SingletonClass::attach(state, klass, super->singleton_class(state));
  }

  void Class::initialize(STATE, Class* klass) {
    Module::initialize(state, klass);
    Class::initialize_data(state, klass);

    klass->instance_type(nil<Fixnum>());
  }

  void Class::initialize(STATE, Class* klass, Class* super) {
    Module::initialize(state, klass);
    Class::initialize_data(state, klass);
    Class::initialize_type(state, klass, super);
  }

  void Class::initialize(STATE, Class* klass, Class* super, const char* name) {
    Class::initialize(state, klass, super, G(object), name);
  }

  void Class::initialize(STATE, Class* klass, Class *super,
      Module* under, const char* name)
  {
    Class::initialize(state, klass, super, under, state->symbol(name));
  }

  void Class::initialize(STATE, Class* klass, Class *super, Module* under, Symbol* name) {
    Module::initialize(state, klass, under, name);
    Class::initialize_data(state, klass);
    Class::initialize_type(state, klass, super);
  }

  void Class::initialize(STATE, Class* klass, Class* super,
      Module* under, Symbol* name, object_type type)
  {
    Module::initialize(state, klass, under, name);
    Class::initialize_data(state, klass);

    klass->superclass(state, super);
    klass->instance_type(Fixnum::from(type));
    klass->type_info(state->memory()->type_info[type]);

    SingletonClass::attach(state, klass, super->singleton_class(state));
  }

  Class* Class::create(STATE, Class* super) {
    return state->memory()->new_class<Class>(state, super);
  }

  Class* Class::create(STATE, Class* super, Module* under, Symbol* name) {
    return state->memory()->new_class<Class>(state, super, under, name);
  }

  Class* Class::s_allocate(STATE) {
    Class* klass = as<Class>(state->memory()->new_object<Class>(state, G(klass)));
    klass->type_info(state->memory()->type_info[ObjectType]);

    return klass;
  }

  namespace {
    Object* allocate_packed(STATE, Class* self) {
      uint32_t size = self->packed_size();

      assert(size > 0);

      PackedObject* obj = state->memory()->new_object<PackedObject>(state, self, size);

      uintptr_t body = reinterpret_cast<uintptr_t>(obj->body_as_array());
      for(size_t i = 0; i < size - sizeof(ObjectHeader);
          i += sizeof(Object*)) {
        Object** pos = reinterpret_cast<Object**>(body + i);
        *pos = cUndef;
      }

      return obj;
    }
  }

  Object* Class::allocate(STATE) {
    Object* obj = cNil;
    object_type obj_type = type_info()->type;

    if(obj_type == PackedObject::type) {
      obj = allocate_packed(state, this);
    } else if(!type_info()->allow_user_allocate || kind_of<SingletonClass>(this)) {
      std::ostringstream msg;
      msg << "direct allocation disabled for ";
      if(kind_of<SingletonClass>(this)) {
         msg << to_string(state);
      } else {
         msg << module_name()->debug_str(state);
      }
      Exception::raise_type_error(state, msg.str().c_str());
    } else if(obj_type == Object::type) {
      auto_pack(state);
      obj = allocate_packed(state, this);
    } else {
      // type_info()->type is neither PackedObject nor Object, so use the
      // generic path.
      obj = state->memory()->new_object<Object>(
          state, this, type_info()->instance_size, obj_type);
    }

#ifdef RBX_ALLOC_TRACKING
    if(unlikely(state->allocation_tracking())) {
      new_obj->setup_allocation_site(state);
    }
#endif

    return obj;
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
      return cNil;
    }

    Class* sup = try_as<Class>(obj);
    if(!sup) {
      return Primitives::failure();
    }

    if(try_as<SingletonClass>(sup)) {
      Exception::raise_type_error(state, "cannot inherit from a singleton class");
    }

    superclass(state, sup);

    instance_type(state, sup->instance_type());
    if(sup->type_info()->type == PackedObject::type) {
      type_info(state->memory()->type_info[ObjectType]);
    } else {
      type_info(sup->type_info());
    }

    SingletonClass::attach(state, this, sup->singleton_class(state));

    return cNil;
  }

  void Class::set_object_type(STATE, size_t type) {
    instance_type(state, Fixnum::from(type));
    type_info(state->memory()->type_info[type]);
  }

  /* Look at this class and it's superclass contents (which includes
   * included modules) and calculate out how to allocate the slots.
   *
   * This locks the class so that construction is serialized.
   */
  void Class::auto_pack(STATE) {
    Class* self = this;
    OnStack<1> os(state, self);

    lock(state);

    // If another thread did this work while we were waiting on the lock,
    // don't redo it.
    if(self->type_info()->type == PackedObject::type) {
      self->unlock(state);
      return;
    }

    size_t slots = 0;

    LookupTable* lt = LookupTable::create(state);

    // If autopacking is enabled, figure out how many slots to use.
    if(state->configuration()->memory_autopack) {
      Module* mod = self;

      int slot = 0;

      while(!mod->nil_p()) {
        Array* info = 0;

        if(Class* cls = try_as<Class>(mod)) {
          info = cls->seen_ivars();
        } else if(IncludedModule* im = try_as<IncludedModule>(mod)) {
          info = im->module()->seen_ivars();
        }

        if(info && !info->nil_p()) {
          for(intptr_t i = 0; i < info->size(); i++) {
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

    self->packed_size(sizeof(Object) + (slots * sizeof(Object*)));
    self->packed_ivar_info(state, lt);

    std::atomic_thread_fence(std::memory_order_acq_rel);
    self->set_object_type(state, PackedObject::type);

    self->unlock(state);
  }

  Class* Class::real_class(STATE, Class* klass) {
    if(SingletonClass* sc = try_as<SingletonClass>(klass)) {
      return sc->true_superclass(state);
    } else {
      return klass;
    }
  }

  void SingletonClass::initialize(STATE, SingletonClass* klass) {
    Module::initialize(state, klass);
    Class::initialize_data(state, klass);

    klass->superclass(state, G(klass));
    klass->set_object_type(state, SingletonClassType);
  }

  SingletonClass* SingletonClass::create(STATE, Object* obj) {
    SingletonClass* sc = state->memory()->new_object<SingletonClass>(state, G(klass));
    sc->object_reference(state, WeakRef::create(state, obj));

    return sc;
  }

  SingletonClass* SingletonClass::attach(STATE, Object* obj, Class* sup) {
    SingletonClass* sc = create(state, obj);

    if(kind_of<PackedObject>(obj)) {
      sc->type_info(state->memory()->type_info[ObjectType]);
    } else {
      sc->type_info(obj->klass()->type_info());
    }

    sc->packed_size(obj->klass()->packed_size());
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
      Class* super = already_sc->true_superclass(state)->singleton_class_instance(state);
      sc->superclass(state, super);
    } else {
      /* If we are attaching to anything but a SingletonClass, the new
       * singleton class's class is the same as its superclass.  This is where
       * the superclass chains for singleton and non-singleton classes
       * diverge.  If no superclass argument was provided, we use the klass we
       * are replacing.
       */
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
    Module* mod = try_as<Module>(cls->singleton());

    std::string name;

    if(mod) {
      name = mod->debug_str(state);
    } else {
      name = "<some object>";
    }

    std::cout << "#<SingletonClass:" <<
      self->class_object(state)->debug_str(state) <<
      " " << name << ":" << (void*)self << ">" << std::endl;
  }
}
