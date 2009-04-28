#include <ruby.h>
#include <math.h>

static VALUE thread_spec_rb_thread_select(VALUE self, VALUE fd_num, VALUE msec) {
  int fd = NUM2INT(fd_num);

  fd_set read;
  FD_ZERO(&read);
  FD_SET(fd, &read);

  struct timeval tv = {0,NUM2INT(msec)};
  int n = rb_thread_select(fd + 1, &read, NULL, NULL, &tv);
  if(n == 1 && FD_ISSET(fd, &read)) return Qtrue;
  return Qfalse;
}

void Init_thread_spec() {
  VALUE cls;
  cls = rb_define_class("CApiThreadSpecs", rb_cObject);
  rb_define_method(cls, "rb_thread_select", thread_spec_rb_thread_select, 2);
}
