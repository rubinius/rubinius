#include "ruby.h"
#include <math.h>

static VALUE thread_spec_rb_thread_select_fd(VALUE self, VALUE fd_num, VALUE msec) {
  int fd = NUM2INT(fd_num);

  fd_set read;
  FD_ZERO(&read);
  FD_SET(fd, &read);

  struct timeval tv = {0, NUM2INT(msec)};
  int n = rb_thread_select(fd + 1, &read, NULL, NULL, &tv);
  if(n == 1 && FD_ISSET(fd, &read)) return Qtrue;
  return Qfalse;
}

static VALUE thread_spec_rb_thread_select(VALUE self, VALUE msec) {
  struct timeval tv = {0, NUM2INT(msec)};
  rb_thread_select(0, NULL, NULL, NULL, &tv);
  return Qnil;
}

static VALUE thread_spec_rb_thread_alone() {
  return INT2NUM(rb_thread_alone());
}

static VALUE thread_spec_rb_thread_current() {
  return rb_thread_current();
}

static VALUE do_unlocked(void* data) {
  if(data == (void*)1) return (VALUE)1;
  return (VALUE)0;
}

// There is really no way to know we're unlocked. So just make sure the arguments
// go through fine.
static VALUE thread_spec_rb_thread_blocking_region() {
  VALUE ret = rb_thread_blocking_region(do_unlocked, (void*)1, 0, 0);
  if(ret == (VALUE)1) return Qtrue;
  return Qfalse;
}

static VALUE thread_spec_rb_thread_local_aref(VALUE self, VALUE thr, VALUE sym) {
  return rb_thread_local_aref(thr, SYM2ID(sym));
}

static VALUE thread_spec_rb_thread_local_aset(VALUE self, VALUE thr, VALUE sym, VALUE value) {
  return rb_thread_local_aset(thr, SYM2ID(sym), value);
}

void Init_thread_spec() {
  VALUE cls;
  cls = rb_define_class("CApiThreadSpecs", rb_cObject);
  rb_define_method(cls, "rb_thread_select_fd", thread_spec_rb_thread_select_fd, 2);
  rb_define_method(cls, "rb_thread_select", thread_spec_rb_thread_select, 1);
  rb_define_method(cls, "rb_thread_alone", thread_spec_rb_thread_alone, 0);
  rb_define_method(cls, "rb_thread_current", thread_spec_rb_thread_current, 0);
  rb_define_method(cls, "rb_thread_blocking_region", thread_spec_rb_thread_blocking_region, 0);
  rb_define_method(cls, "rb_thread_local_aref", thread_spec_rb_thread_local_aref, 2);
  rb_define_method(cls, "rb_thread_local_aset", thread_spec_rb_thread_local_aset, 3);
}
