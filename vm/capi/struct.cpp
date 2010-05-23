#include "builtin/array.hpp"

#include "capi/capi.hpp"
#include "capi/include/ruby.h"

#include <stdarg.h>

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  VALUE rb_struct_define(const char *name, ...) {
    va_list args;
    va_start(args, name);

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    VALUE nm;
    if(name) {
      nm = rb_str_new2(name);
    } else {
      nm = Qnil;
    }

    char *sym;
    Array* array = Array::create(env->state(), 0);
    while((sym = va_arg(args, char*)) != 0) {
      array->append(env->state(), env->get_object(rb_intern(sym)));
    }

    va_end(args);

    return rb_funcall(rb_cStruct, rb_intern("make_struct"), 2, nm, env->get_handle(array));
  }
}
