#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {

  VALUE rb_range_new(VALUE begin, VALUE end, int exclude_end) {
    VALUE exclude_end_handle = exclude_end ? Qtrue : Qfalse;
    return rb_funcall(rb_cRange, rb_intern("new"), 3, begin, end, exclude_end_handle);
  }
}
