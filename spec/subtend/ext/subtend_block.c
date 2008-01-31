#include <ruby.h>

static VALUE sb_test(VALUE self) {
  if(rb_block_given_p()) return Qtrue;
  return Qfalse;
}

static VALUE sb_yield(VALUE self) {
  return rb_yield(5);
}

void Init_subtend_block() {
  VALUE cls;
  cls = rb_define_class("SubtendBlock", rb_cObject);

  rb_define_method(cls, "block_given?", sb_test, 0);
  rb_define_method(cls, "do_yield", sb_yield, 0);
}
