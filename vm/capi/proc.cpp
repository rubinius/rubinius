#include "builtin/native_method.hpp"
#include "builtin/proc.hpp"

#include "configuration.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  VALUE rb_proc_new(VALUE (*func)(ANYARGS), VALUE val) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    int arity = ITERATE_BLOCK;
    Proc* prc = capi::wrap_c_function((void*)func, val, arity);

    return env->get_handle(prc);
  }
}
