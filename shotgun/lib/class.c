#include "shotgun.h"
#include "module.h"

OBJECT class_new(STATE, char *name, int fields, OBJECT sup, OBJECT ns) {
  OBJECT cls;
  
  cls = class_allocate(state);
  class_set_instance_fields(cls, I2N(fields));
  class_set_superclass(cls, sup);
  module_setup_with_namespace(state, cls, name, ns);
  return cls;
}

OBJECT class_new_instance(STATE, OBJECT self) {
  int count;
  
  count = FIXNUM_TO_INT(class_get_instance_fields(self));
  return NEW_OBJECT(self, count);
}
