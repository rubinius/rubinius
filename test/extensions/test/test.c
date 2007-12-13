
#include <ruby.h>
#include <stdio.h>

VALUE test_print(VALUE self) {
  printf("C* Hello from the C world! %p\n", self);
  return Qnil;
}

VALUE test_call(VALUE self) {
  VALUE tmp;
  printf("C* calling into ruby...\n");
  tmp = rb_funcall(self, rb_intern("from_c"), 0);
  printf("C* after the call!\n");
  return tmp;
}

VALUE test_call2(VALUE self) {
  printf("C* calling into ruby...\n");
  rb_funcall(self, rb_intern("from_c2"), 0);
  printf("C* after the call!\n");
  return Qtrue;
}

VALUE test_call3(VALUE self) {
  printf("C* calling into ruby...\n");
  rb_funcall(self, rb_intern("from_c3"), 0);
  printf("C* after the call!\n");
  return Qtrue;
}

VALUE test_arg1(VALUE self, VALUE arg, VALUE arg2) {
  printf("C* call arg on self...\n");
  rb_funcall(self, SYM2ID(arg), 1, arg2);
  printf("C* done with arg call.\n");
  return Qnil;
}

VALUE test_segfault(VALUE self) {
  char *i = (char*)4;
  char x;
  printf("C* trying to induce segfault...\n");
  x = *i;
  return Qnil;
}

VALUE test_vargs(VALUE self, int argc, VALUE *argv) {
  printf("C* %d args, self: %p, argv: %p\n", argc, self, argv);
  printf("C* 0: %p, 1: %p\n", argv[0], argv[1]);
  return argv[1];
}

VALUE test_vargs2(VALUE self, VALUE ary) {
  printf("C* %d args\n", rb_ary_size(ary));  
  return rb_ary_entry(ary, 0);
}

void Init_test() {
  VALUE cls;
  
  printf("C* Loading test extension..\n");

  cls = rb_define_class("TestExtension", rb_cObject);
  rb_define_method(cls, "test_print", test_print, 0);
  rb_define_method(cls, "test_call", test_call, 0);
  rb_define_method(cls, "test_call2", test_call2, 0);
  rb_define_method(cls, "test_call3", test_call3, 0);
  rb_define_method(cls, "test_arg1", test_arg1, 2);
  rb_define_method(cls, "test_segfault", test_segfault, 0);
  rb_define_method(cls, "test_vargs", test_vargs, -1);
  rb_define_method(cls, "test_vargs2", test_vargs2, -2);
  
  printf("C* Added TestExtension class.\n");
}
