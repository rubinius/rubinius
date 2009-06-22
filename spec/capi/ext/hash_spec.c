#include "ruby.h"

VALUE hash_spec_rb_hash_new(VALUE self) {
  return rb_hash_new();
}

VALUE hash_spec_rb_hash_aref(VALUE self, VALUE hash, VALUE key) {
  return rb_hash_aref(hash, key);
}

VALUE hash_spec_rb_hash_aref_nil(VALUE self, VALUE hash, VALUE key) {
  VALUE ret = rb_hash_aref(hash, key);
  return ret == Qnil ? Qtrue : Qfalse;
}

VALUE hash_spec_rb_hash_aset(VALUE self, VALUE hash, VALUE key, VALUE val) {
  return rb_hash_aset(hash, key, val);
}

VALUE hash_spec_rb_hash_delete(VALUE self, VALUE hash, VALUE key) {
  return rb_hash_delete(hash, key);
}

#ifdef RUBINIUS
// rb_hash_size is a static symbol in MRI
VALUE hash_spec_rb_hash_size(VALUE self, VALUE hash) {
  return rb_hash_size(hash);
}
#endif

void Init_hash_spec() {
  VALUE cls;
  cls = rb_define_class("CApiHashSpecs", rb_cObject);
  rb_define_method(cls, "rb_hash_new", hash_spec_rb_hash_new, 0);
  rb_define_method(cls, "rb_hash_aref", hash_spec_rb_hash_aref, 2);
  rb_define_method(cls, "rb_hash_aref_nil", hash_spec_rb_hash_aref_nil, 2);
  rb_define_method(cls, "rb_hash_aset", hash_spec_rb_hash_aset, 3);
  rb_define_method(cls, "rb_hash_delete", hash_spec_rb_hash_delete, 2);

#ifdef RUBINIUS
  rb_define_method(cls, "rb_hash_size", hash_spec_rb_hash_size, 1);
#endif
}
