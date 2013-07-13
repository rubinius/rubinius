#include "builtin/nativemethod.hpp"
#include "builtin/proc.hpp"

#include "configuration.hpp"
#include "version.h"

#include "capi/capi.hpp"
#include "capi/18/include/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  VALUE rb_proc_new(VALUE (*func)(ANYARGS), VALUE val) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    int arity = LANGUAGE_18_ENABLED ? C_LAMBDA : ITERATE_BLOCK;
    Proc* prc = capi::wrap_c_function((void*)func, val, arity);

    return env->get_handle(prc);
  }
}
