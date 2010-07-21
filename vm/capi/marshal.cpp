#include "capi/capi.hpp"
#include "capi/include/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  VALUE rb_marshal_dump(VALUE obj, VALUE io) {
    int argc = 1;
    VALUE argv[2];
    VALUE marshal = rb_const_get(rb_cObject, rb_intern("Marshal"));

    argv[0] = obj;

    if (!NIL_P(io)) {
      argv[1] = io;
      argc = 2;
    }

    return rb_funcall2(marshal, rb_intern("dump"), argc, argv);
  }

  VALUE rb_marshal_load(VALUE data) {
    return rb_funcall(rb_path2class("Marshal"), rb_intern("load"), 1, data);
  }

}
