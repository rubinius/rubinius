#include "builtin/array.hpp"

#include "capi/capi.hpp"
#include "capi/18/include/ruby.h"

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

  VALUE rb_struct_new(VALUE klass, ...) {
    va_list args;
    va_start(args, klass);

    VALUE sz = rb_funcall2(klass, rb_intern("length"), 0, 0);
    int total = FIX2INT(sz);

    VALUE mem[total];
    for(int i = 0; i < total; i++) {
      mem[i] = va_arg(args, VALUE);
    }

    va_end(args);

    return rb_funcall2(klass, rb_intern("new"), total, mem);
  }

  VALUE rb_struct_aref(VALUE struct_handle, VALUE key) {
    return rb_funcall(struct_handle, rb_intern("[]"), 1, key);
  }

  VALUE rb_struct_getmember(VALUE struct_handle, ID key) {
    return rb_funcall(struct_handle, key, 0);
  }

  VALUE rb_struct_aset(VALUE struct_handle, VALUE key, VALUE value) {
    return rb_funcall(struct_handle, rb_intern("[]="), 2, key, value);
  }
}
