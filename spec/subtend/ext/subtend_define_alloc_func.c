#include <ruby.h>
#include <string.h>

struct sample_wrapped_struct {
    int foo;
};

VALUE sdaf_alloc_func(VALUE klass) {
    struct sample_wrapped_struct* bar = (struct sample_wrapped_struct *)malloc(sizeof(struct sample_wrapped_struct));
    bar->foo = 42;
    return Data_Wrap_Struct(klass, NULL, NULL, bar);
}

VALUE sdaf_get_struct(VALUE self) {
    struct sample_wrapped_struct* bar;
    Data_Get_Struct(self, struct sample_wrapped_struct, bar);
    
    return INT2FIX((*bar).foo);
}

void Init_subtend_define_alloc_func() {
  VALUE cls;
  cls = rb_define_class("SubtendAlloc", rb_cObject);
  rb_define_alloc_func(cls, sdaf_alloc_func);
  rb_define_method(cls, "wrapped_data", sdaf_get_struct, 0);
}