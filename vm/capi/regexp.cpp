#include "vm.hpp"
#include "vm/object_utils.hpp"

#include "builtin/array.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/io.hpp"
#include "objectmemory.hpp"
#include "primitives.hpp"

#include "capi/capi.hpp"
#include "capi/include/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {

  VALUE rb_reg_source(VALUE r) {
    return rb_String(rb_funcall(r, rb_intern("source"), 0));
  }

  int rb_reg_options(VALUE r) {
    return FIX2INT(rb_funcall(r, rb_intern("options"), 0));
  }

  VALUE rb_reg_regcomp(VALUE str) {
    return rb_funcall(rb_cRegexp, rb_intern("new"), 1, str);
  }
}
