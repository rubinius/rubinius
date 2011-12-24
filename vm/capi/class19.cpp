#include "builtin/nativemethod.hpp"
#include "builtin/string.hpp"

#include "capi/capi.hpp"
#include "capi/19/include/ruby/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  VALUE rb_path_to_class(VALUE str) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    String* string = c_as<String>(env->get_object(str));

    return rb_path2class(string->c_str(env->state()));
  }

  VALUE rb_class_superclass(VALUE klass) {
    return rb_funcall(klass, rb_intern("superclass"), 0);
  }
}
