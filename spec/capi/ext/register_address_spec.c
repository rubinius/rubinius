#include "ruby.h"

VALUE registered_tagged_value;
VALUE registered_reference_value;

static VALUE registered_tagged_address(VALUE self) {
  return registered_tagged_value;
}

static VALUE registered_reference_address(VALUE self) {
  return registered_reference_value;
}

void Init_register_address_spec() {
  registered_tagged_value    = INT2NUM(10);
  registered_reference_value = rb_str_new2("Globally registered data");
  rb_gc_register_address(&registered_tagged_value);
  rb_gc_register_address(&registered_reference_value);

  VALUE cls;
  cls = rb_define_class("CApiRegisterAddressSpecs", rb_cObject);

  rb_define_method(cls, "registered_tagged_address", registered_tagged_address, 0);
  rb_define_method(cls, "registered_reference_address", registered_reference_address, 0);
}
