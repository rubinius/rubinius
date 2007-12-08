#include <ruby.h>
#include <string.h>

struct sample_wrapped_struct {
    int foo;
};

VALUE sws_wrap_struct(VALUE self, VALUE val) {
    struct sample_wrapped_struct* bar = (struct sample_wrapped_struct *)malloc(sizeof(struct sample_wrapped_struct));
    bar->foo = FIX2INT(val);
    return Data_Wrap_Struct(rb_cObject, NULL, NULL, bar);
}

VALUE sws_get_struct(VALUE self, VALUE obj) {
    struct sample_wrapped_struct* bar;
    Data_Get_Struct(obj, struct sample_wrapped_struct, bar);
    
    return INT2FIX((*bar).foo);
}

void Init_subtend_wrapped_struct() {
  VALUE cls;
  cls = rb_define_class("SubtendWrappedStruct", rb_cObject);
  rb_define_method(cls, "wrap_struct", sws_wrap_struct, 1);
  rb_define_method(cls, "get_struct", sws_get_struct, 1);
}