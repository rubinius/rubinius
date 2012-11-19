#include "capi/capi.hpp"
#include "capi/18/include/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  VALUE rb_enumeratorize(VALUE obj, VALUE meth, int argc, VALUE *argv) {
    VALUE args[argc + 2];

    args[0] = obj;
    args[1] = meth;

    for(int i = 0; i < argc; i++) {
      args[i + 2] = argv[i];
    }

    return rb_funcall2(rb_cEnumerator, rb_intern("new"), argc + 2, args);
  }
}

