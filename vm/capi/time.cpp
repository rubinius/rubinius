#include <math.h>
#include <cmath>

#include "builtin/object.hpp"

#include "capi/capi.hpp"
#include "capi/include/ruby.h"
#include "builtin/time.hpp"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  VALUE rb_time_new(time_t sec, time_t usec) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return env->get_handle(Time::specific(env->state(),
                                          Integer::from(env->state(), sec),
                                          Integer::from(env->state(), usec),
                                          Qfalse));
  }

}
