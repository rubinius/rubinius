extern "C" {
#include "ruby.h"
}
#include "rubyspec.h"

#ifdef RUBY_VERSION_IS_1_9
#include "ruby/st.h"
#else
#ifdef __cplusplus
extern "C" {
#endif
#include "st.h"
#ifdef __cplusplus
}
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif


#ifdef HAVE_ST_INIT_STRTABLE
VALUE st_spec_alloc(VALUE self) {
  VALUE obj;
  st_table *table = st_init_numtable();
  obj = Data_Wrap_Struct(self, 0, st_free_table, table);
  return obj;
}
#endif

#ifdef HAVE_ST_INSERT
VALUE st_spec_insert(VALUE self, VALUE key, VALUE value) {
  st_table *table;
  int res;
  Data_Get_Struct(self, st_table, table);
  res = st_insert(table, NUM2LONG(key), value);
  return INT2NUM(res);
}
#endif

#ifdef HAVE_ST_LOOKUP
VALUE st_spec_lookup(VALUE self, VALUE key) {
  st_table *table;
  VALUE value;
  int res;
  Data_Get_Struct(self, st_table, table);
  res = st_lookup(table, NUM2LONG(key), (st_data_t*)&value);
  return res ? value : Qnil;
}
#endif

#ifdef HAVE_ST_DELETE
VALUE st_spec_delete(VALUE self, VALUE key) {
  st_table *table;
  VALUE value;
  int res;
  st_data_t key2 = NUM2LONG(key);
  Data_Get_Struct(self, st_table, table);
  res = st_delete(table, &key2, (st_data_t*)&value);
  return res ? value : Qnil;
}
#endif

#ifdef HAVE_ST_FOREACH
int st_spec_foreach_i(VALUE key, VALUE value, VALUE other) {
  rb_hash_aset(other, LONG2NUM(key), value);
  return ST_CONTINUE;
}

int st_spec_foreach_stop_i(VALUE key, VALUE value, VALUE other) {
  rb_hash_aset(other, LONG2NUM(key), value);
  return ST_STOP;
}

int st_spec_foreach_delete_i(VALUE key, VALUE value, VALUE other) {
  rb_hash_aset(other, LONG2NUM(key), value);
  return ST_DELETE;
}

VALUE st_spec_foreach(VALUE self) {
  st_table *table;
  VALUE other = rb_hash_new();
  Data_Get_Struct(self, st_table, table);
  st_foreach(table, (int (*)(...))st_spec_foreach_i, other);
  return other;
}

VALUE st_spec_foreach_stop(VALUE self) {
  st_table *table;
  VALUE other = rb_hash_new();
  Data_Get_Struct(self, st_table, table);
  st_foreach(table, (int (*)(...))st_spec_foreach_stop_i, other);
  return other;
}

VALUE st_spec_foreach_delete(VALUE self) {
  st_table *table;
  VALUE other = rb_hash_new();
  Data_Get_Struct(self, st_table, table);
  st_foreach(table, (int (*)(...))st_spec_foreach_delete_i, other);
  return other;
}
#endif


void Init_st_spec() {
  VALUE cls;
  cls = rb_define_class("CApiStSpecs", rb_cObject);
#ifdef HAVE_ST_INIT_STRTABLE
  rb_define_alloc_func(cls, st_spec_alloc);
#endif

#ifdef HAVE_ST_INSERT
  rb_define_method(cls, "st_insert", RUBY_METHOD_FUNC(st_spec_insert), 2);
#endif

#ifdef HAVE_ST_LOOKUP
  rb_define_method(cls, "st_lookup", RUBY_METHOD_FUNC(st_spec_lookup), 1);
#endif

#ifdef HAVE_ST_DELETE
  rb_define_method(cls, "st_delete", RUBY_METHOD_FUNC(st_spec_delete), 1);
#endif

#ifdef HAVE_ST_FOREACH
  rb_define_method(cls, "st_foreach", RUBY_METHOD_FUNC(st_spec_foreach), 0);
  rb_define_method(cls, "st_foreach_stop", RUBY_METHOD_FUNC(st_spec_foreach_stop), 0);
  rb_define_method(cls, "st_foreach_delete", RUBY_METHOD_FUNC(st_spec_foreach_delete), 0);
#endif

}

#ifdef __cplusplus
}
#endif
