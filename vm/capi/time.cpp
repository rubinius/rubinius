#include <math.h>

#include "builtin/object.hpp"
#include "builtin/time.hpp"

#include "capi/capi.hpp"
#include "capi/18/include/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  VALUE rb_time_new(time_t sec, long usec) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Class* cls = env->state()->vm()->shared.globals.time_class.get();

    // Prevent overflow before multiplying.
    if(usec >= 1000000) {
      sec += usec / 1000000;
      usec %= 1000000;
    }

    Time* obj = Time::specific(env->state(), cls,
                                Integer::from(env->state(), sec),
                                Integer::from(env->state(), usec * 1000),
                                cFalse, cNil);
    return env->get_handle(obj);
  }

  VALUE rb_time_nano_new(time_t sec, long nsec) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Class* cls = env->state()->vm()->shared.globals.time_class.get();

    // Prevent overflow before multiplying.
    if(nsec >= 1000000000) {
      sec += nsec / 1000000000;
      nsec %= 1000000000;
    }

    Time* obj = Time::specific(env->state(), cls,
                                Integer::from(env->state(), sec),
                                Integer::from(env->state(), nsec),
                                cFalse, cNil);
    return env->get_handle(obj);
  }

}
