#include "builtin/nativemethod.hpp"
#include "builtin/proc.hpp"

#include "capi/capi.hpp"
#include "capi/18/include/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  VALUE rb_proc_new(VALUE (*func)(ANYARGS), VALUE val) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Proc* prc = capi::wrap_c_function((void*)func, val, C_LAMBDA);

    return env->get_handle(prc);
  }
}
