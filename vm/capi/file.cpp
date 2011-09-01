#include "vm.hpp"
#include "vm/object_utils.hpp"

#include "capi/capi.hpp"
#include "capi/18/include/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  VALUE rb_file_open(const char* name, const char* mode) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    VALUE n = env->get_handle(String::create(env->state(), name));
    VALUE m = env->get_handle(String::create(env->state(), mode));

    return rb_funcall(rb_cFile, rb_intern("open"), 2, n, m);
  }
}
