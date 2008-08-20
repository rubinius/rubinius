#include <ruby.h>
#include <intern.h>

static VALUE sa_cv_set(VALUE self, VALUE klass, VALUE name, VALUE val) {
	return rb_cv_set(klass, StringValuePtr(name), val);
}

static VALUE sa_cvar_set(VALUE self, VALUE klass, VALUE name, VALUE val) {
	return rb_cvar_set(klass, rb_intern(StringValuePtr(name)), val);
}

static VALUE sa_cv_get(VALUE self, VALUE klass, VALUE name) {
	return rb_cv_get(klass, StringValuePtr(name));
}

static VALUE sa_cvar_get(VALUE self, VALUE klass, VALUE name) {
	return rb_cvar_get(klass, rb_intern(StringValuePtr(name)));
}

void Init_subtend_cv_get_set() {
  VALUE cls;
  cls = rb_define_class("SubtendCVGetSet", rb_cObject);
  rb_define_method(cls, "rb_cv_set", sa_cv_set, 3);
  rb_define_method(cls, "rb_cvar_set", sa_cvar_set, 3);
  rb_define_method(cls, "rb_cv_get", sa_cv_get, 2);
  rb_define_method(cls, "rb_cvar_get", sa_cvar_get, 2);
}
