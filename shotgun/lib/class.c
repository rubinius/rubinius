#include "shotgun.h"
#include "module.h"
#include "class.h"

OBJECT class_superclass(STATE, OBJECT cls) {
  OBJECT sup;
  
  sup = class_get_superclass(cls);
  
  while(!ISA(sup, BASIC_CLASS(class))) {
    sup = class_get_superclass(sup);
  }
  
  return sup;
}

OBJECT class_new(STATE, const char *name, int fields, OBJECT sup, OBJECT ns) {
  OBJECT cls;
  
  cls = class_create(state);
  class_set_instance_fields(cls, I2N(fields));
  class_set_superclass(cls, sup);
  module_setup_fields(state, cls);
  object_create_metaclass(state, cls, object_metaclass(state, sup));
  module_setup_name(state, cls, name, ns);
  return cls;
}

OBJECT class_new_instance(STATE, OBJECT self) {
  int count;
  
  count = FIXNUM_TO_INT(class_get_instance_fields(self));
  return NEW_OBJECT(self, count);
}

OBJECT class_constitute(STATE, OBJECT sup, OBJECT under) {
  OBJECT val, sup_itr;
    
  if(NIL_P(sup)) {
    sup = state->global->object;
  } else if(sup == Qfalse) {
    /* Support class detached from the normal class heirarchy. */
    sup = Qnil;
  } else if(!ISA(sup, state->global->class)) {
    /* Validate sup is a valid superclass-like object. */
    
    sup_itr = sup;
    while(!NIL_P(sup_itr)) {
      if(NUM_FIELDS(sup_itr) <= CLASS_f_SUPERCLASS ||
         !ISA(class_get_methods(sup_itr), state->global->hash)) {
        /* Ok, this wont work as a superclass. */
        return Qnil;
      } else {
        sup_itr = class_get_superclass(sup_itr);
      }
    }
    
    /* Ok, we validated the hierarchy as being superclass-like, so it's
       ok to use. */
  }
  
  val = class_create(state);
  
  /* Push superclass instance information down. */
  if(NIL_P(sup) || NUM_FIELDS(sup) <= CLASS_f_INSTANCE_FIELDS) {
    /* When this object is detatched from the normal class hierarchy, we give
       it the normal fields and flags info by default. */
    class_set_instance_fields(val, class_get_instance_fields(state->global->object));
    class_set_has_ivars(val, class_get_has_ivars(state->global->object));
    class_set_needs_cleanup(val, class_get_needs_cleanup(state->global->object));
    class_set_object_type(val, class_get_object_type(state->global->object));
  } else {
    class_set_instance_fields(val, class_get_instance_fields(sup));
    class_set_has_ivars(val, class_get_has_ivars(sup));
    class_set_needs_cleanup(val, class_get_needs_cleanup(sup));
    class_set_object_type(val, class_get_object_type(sup));
  }
  
  // printf("Setting superclass of %p to: %p\n", val, sup);
  class_set_superclass(val, sup);
  module_setup_fields(state, val);
  object_create_metaclass(state, val, object_metaclass(state, sup));
  module_set_parent(val, under);
  module_setup_fields(state, object_metaclass(state, val));
  return val; 
}
