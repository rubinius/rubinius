#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  VALUE rb_Rational(VALUE num, VALUE den) {
    return rb_funcall(rb_cRational, rb_intern("convert"), 2, num, den);
  }

  VALUE rb_rational_new(VALUE num, VALUE den) {
    return rb_funcall(rb_mKernel, rb_intern("Rational"), 2, num, den);
  }
}
