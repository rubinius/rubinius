#include "vm.hpp"
#include "vm/object_utils.hpp"

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
}
