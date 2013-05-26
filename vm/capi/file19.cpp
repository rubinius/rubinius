#include "builtin/string.hpp"
#include "capi/capi.hpp"
#include "capi/19/include/ruby/ruby.h"

#include <stdio.h>

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  namespace capi {
  }
}

extern "C" {
  FILE *  rb_io_stdio_file(rb_io_t *fptr) {
    return fptr->f;
  }

  VALUE rb_file_path_value(volatile VALUE* obj) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    STATE = env->state();

    if(!kind_of<String>(env->get_object(*obj))) {
      *obj = rb_funcall(env->get_handle(G(type)), rb_intern("coerce_to_path"), 1, *obj);
    }

    return *obj;
  }

  VALUE rb_file_open_str(VALUE name, const char* mode) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    VALUE m = env->get_handle(String::create(env->state(), mode));

    FilePathValue(name);
    return rb_funcall(rb_cFile, rb_intern("open"), 2, name, m);
  }
}
