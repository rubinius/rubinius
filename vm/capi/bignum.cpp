#include "capi/ruby.h"

extern "C" {
  VALUE rb_big2str(VALUE self, int base) {
    return rb_funcall(self, rb_intern("to_s"), 1, INT2FIX(base));
  }
}
