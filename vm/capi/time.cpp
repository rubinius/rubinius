#include <math.h>

#include "builtin/object.hpp"
#include "builtin/time.hpp"

#include "capi/capi.hpp"
#include "capi/include/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  VALUE rb_time_new(time_t sec, time_t usec) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Class* cls = env->state()->shared.globals.time_class.get();
    Time* obj = Time::specific(env->state(), cls,
                                Integer::from(env->state(), sec),
                                Integer::from(env->state(), usec),
                                Qfalse);
    return env->get_handle(obj);
  }

}
