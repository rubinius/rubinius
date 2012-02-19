#include "builtin/nativemethod.hpp"
#include "builtin/proc.hpp"

#include "vm/configuration.hpp"

#include "capi/capi.hpp"
#include "capi/18/include/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  VALUE rb_proc_new(VALUE (*func)(ANYARGS), VALUE val) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    int arity = LANGUAGE_18_ENABLED(env->state()) ? C_LAMBDA : ITERATE_BLOCK;
    Proc* prc = capi::wrap_c_function((void*)func, val, arity);

    return env->get_handle(prc);
  }
}
