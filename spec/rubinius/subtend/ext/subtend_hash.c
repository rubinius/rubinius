#include <ruby.h>

VALUE sh_new_hash(VALUE self) {
  return rb_hash_new();
}

VALUE sh_access(VALUE self, VALUE hash, VALUE key) {  
  return rb_hash_aref(hash, key);
}

VALUE sh_insert(VALUE self, VALUE hash, VALUE key, VALUE val) {
  return rb_hash_aset(hash, key, val);
}

VALUE sh_remove(VALUE self, VALUE hash, VALUE key) {
  return rb_hash_delete(hash, key);
}

void Init_subtend_hash() {
  VALUE cls;
  cls = rb_define_class("SubtendHash", rb_cObject);
  rb_define_method(cls, "new_hash", sh_new_hash, 0);
  rb_define_method(cls, "access", sh_access, 2);
  rb_define_method(cls, "insert", sh_insert, 3);
  rb_define_method(cls, "remove", sh_remove, 2);
}
