#include <ruby.h>
#include <math.h>

static VALUE sf_read_ready(VALUE self, VALUE io) {
  int fd = NUM2INT(io);

  fd_set read;
  FD_ZERO(&read);
  FD_SET(fd, &read);

  struct timeval tv = {0,0};
  int n = rb_thread_select(fd + 1, &read, NULL, NULL, &tv);
  if(n == 1 && FD_ISSET(fd, &read)) return Qtrue;
  return Qfalse;
}

static VALUE sf_sleep(VALUE self) {
  struct timeval tv = {2,0};

  rb_thread_select(0, NULL, NULL, NULL, &tv);
  return Qnil;
}

void Init_thread_spec() {
  VALUE cls;
  cls = rb_define_class("CApiThreadSpecs", rb_cObject);
  rb_define_method(cls, "sf_read_ready", sf_read_ready, 1);
  rb_define_method(cls, "sf_sleep", sf_sleep, 0);
}
