#include "capi/capi.hpp"
#include "capi/ruby.h"

extern "C" {
  VALUE rb_io_write(VALUE io, VALUE str) {
    return rb_funcall(io, rb_intern("write"), 1, str);
  }
}
