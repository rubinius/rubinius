#include <math.h>

#include "builtin/object.hpp"
#include "builtin/time.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {

  struct timespec capi_time_num_timespec(NativeMethodEnvironment* env, VALUE num) {
    struct timespec ts;

    if(FIXNUM_P(num)) {
      ts.tv_sec = NUM2TIMET(num);
      ts.tv_nsec = 0;
    } else {
      num = rb_funcall(env->get_handle(env->state()->globals().type.get()), rb_intern("coerce_to_exact_num"), 1, num);

      VALUE parts = rb_funcall(num, rb_intern("divmod"), 1, INT2FIX(1));
      ts.tv_sec = NUM2TIMET(rb_ary_entry(parts, 0));
      ts.tv_nsec = NUM2LONG(rb_funcall(rb_ary_entry(parts, 1), rb_intern("*"), 1, INT2FIX(1000000000)));
    }
    return ts;
  }

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

  VALUE rb_time_num_new(VALUE num, VALUE offset) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Class* cls = env->state()->vm()->shared.globals.time_class.get();

    struct timespec ts = capi_time_num_timespec(env, num);

    State* state = env->state();
    Time* obj = Time::specific(state, cls, Integer::from(state, ts.tv_sec), Integer::from(state, ts.tv_nsec), cFalse, env->get_object(offset));
    return env->get_handle(obj);
  }

  struct timeval rb_time_interval(VALUE time) {
    struct timeval tv;

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    if(kind_of<Time>(env->get_object(time))) {
      rb_raise(rb_eTypeError, "can't use Time for time interval");
    }
    struct timespec ts = capi_time_num_timespec(env, time);

    tv.tv_sec = ts.tv_sec;
    tv.tv_usec = ts.tv_nsec / 1000;
    if(tv.tv_sec < 0) {
      rb_raise(rb_eArgError, "negative value invalid for time interval");
    }
    return tv;
  }

  struct timeval rb_time_timeval(VALUE time) {
    struct timeval tv;

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    struct timespec ts = capi_time_num_timespec(env, time);

    tv.tv_sec = ts.tv_sec;
    tv.tv_usec = ts.tv_nsec / 1000;
    return tv;
  }

  struct timespec rb_time_timespec(VALUE time) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    struct timespec ts = capi_time_num_timespec(env, time);
    return ts;
  }
}
