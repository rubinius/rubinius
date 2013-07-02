#include "capi/capi.hpp"
#include "capi/19/include/ruby/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  VALUE rb_Complex(VALUE real, VALUE imag) {
    return rb_funcall(rb_cComplex, rb_intern("convert"), 2, real, imag);
  }

  VALUE rb_complex_new(VALUE real, VALUE imag) {
    return rb_funcall(rb_mKernel, rb_intern("Complex"), 2, real, imag);
  }
}

