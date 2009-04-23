#include "capi/capi.hpp"
#include "capi/ruby.h"

#include "builtin/float.hpp"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  VALUE rb_float_new(double val) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return env->get_handle(Float::create(env->state(), val));
  }
}
