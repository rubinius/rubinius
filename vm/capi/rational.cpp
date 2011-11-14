#include "capi/capi.hpp"
#include "capi/19/include/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  VALUE rb_Rational(VALUE num, VALUE den)
  {
      return rb_funcall(rb_cRational, rb_intern("convert"), 2, num, den);
  }
}
