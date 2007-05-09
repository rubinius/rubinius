#include <ruby.h>

VALUE sh_new_hash(VALUE self) {
  return rb_hash_new();
}

VALUE sh_access(VALUE self, VALUE hash, VALUE key) {  
  return rb_hash_aref(hash, key);
}

void Init_subtend_hash() {
  VALUE cls;
  cls = rb_define_class("SubtendHash", rb_cObject);
  rb_define_method(cls, "new_hash", sh_new_hash, 0);
  rb_define_method(cls, "access", sh_access, 2);
}
