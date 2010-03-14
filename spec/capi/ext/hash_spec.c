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

VALUE hash_spec_rb_hash_lookup(VALUE self, VALUE hash, VALUE key) {
  return rb_hash_lookup(hash, key);
}

VALUE hash_spec_rb_hash_lookup_nil(VALUE self, VALUE hash, VALUE key) {
  VALUE ret = rb_hash_lookup(hash, key);
  return ret == Qnil ? Qtrue : Qfalse;
}

VALUE hash_spec_rb_hash_aset(VALUE self, VALUE hash, VALUE key, VALUE val) {
  return rb_hash_aset(hash, key, val);
}

VALUE hash_spec_rb_hash_delete(VALUE self, VALUE hash, VALUE key) {
  return rb_hash_delete(hash, key);
}

static int foreach_i(VALUE key, VALUE val, VALUE other) {
  rb_hash_aset(other, key, val);
  return 0; // ST_CONTINUE;
}

VALUE hash_spec_rb_hash_foreach(VALUE self, VALUE hsh) {
  VALUE other = rb_hash_new();
  rb_hash_foreach(hsh, foreach_i, other);
  return other;
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
  rb_define_method(cls, "rb_hash_lookup", hash_spec_rb_hash_lookup, 2);
  rb_define_method(cls, "rb_hash_lookup_nil", hash_spec_rb_hash_lookup_nil, 2);
  rb_define_method(cls, "rb_hash_aset", hash_spec_rb_hash_aset, 3);
  rb_define_method(cls, "rb_hash_delete", hash_spec_rb_hash_delete, 2);
  rb_define_method(cls, "rb_hash_foreach", hash_spec_rb_hash_foreach, 1);

#ifdef RUBINIUS
  rb_define_method(cls, "rb_hash_size", hash_spec_rb_hash_size, 1);
#endif
}
