#include <math.h>

#include "class/object.hpp"
#include "class/time.hpp"

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
      num = rb_funcall(MemoryHandle::value(
            env->state()->globals().type.get()), rb_intern("coerce_to_exact_num"), 1, num);

      VALUE parts = rb_funcall(num, rb_intern("divmod"), 1, INT2FIX(1));
      ts.tv_sec = NUM2TIMET(rb_ary_entry(parts, 0));
      ts.tv_nsec = NUM2LONG(rb_funcall(rb_ary_entry(parts, 1), rb_intern("*"), 1, INT2FIX(1000000000)));
    }
    return ts;
  }

  VALUE rb_time_new(time_t sec, long usec) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Class* cls = env->state()->memory()->globals.time_class.get();

    // Prevent overflow before multiplying.
    if(usec >= 1000000) {
      sec += usec / 1000000;
      usec %= 1000000;
    }

    Time* obj = Time::specific(env->state(), cls,
                                Integer::from(env->state(), sec),
                                Integer::from(env->state(), usec * 1000),
                                cFalse, cNil);
    return MemoryHandle::value(obj);
  }

  VALUE rb_time_nano_new(time_t sec, long nsec) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Class* cls = env->state()->memory()->globals.time_class.get();

    // Prevent overflow before multiplying.
    if(nsec >= 1000000000) {
      sec += nsec / 1000000000;
      nsec %= 1000000000;
    }

    Time* obj = Time::specific(env->state(), cls,
                                Integer::from(env->state(), sec),
                                Integer::from(env->state(), nsec),
                                cFalse, cNil);
    return MemoryHandle::value(obj);
  }

  VALUE rb_time_num_new(VALUE num, VALUE offset) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Class* cls = env->state()->memory()->globals.time_class.get();

    struct timespec ts = capi_time_num_timespec(env, num);

    ThreadState* state = env->state();
    Time* obj = Time::specific(state, cls,
        Integer::from(state, ts.tv_sec),
        Integer::from(state, ts.tv_nsec),
        cFalse, MemoryHandle::object(offset));
    return MemoryHandle::value(obj);
  }

  struct timeval rb_time_interval(VALUE time) {
    struct timeval tv;

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    if(MemoryHandle::try_as<Time>(time)) {
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

  VALUE rb_time_timespec_new(const struct timespec *ts, int offset) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Time* time = Time::at(env->state(), ts->tv_sec, ts->tv_nsec);

    if(-86400 < offset && offset < 86400) { /* fixed offset */
      time->offset(env->state(), Fixnum::from(offset));
      time->is_gmt(env->state(), cFalse);
      time->zone(env->state(), cNil);
    } else if(offset == INT_MAX) { /* localtime */
    } else if(offset == INT_MAX - 1) { /* UTC */
      time->is_gmt(env->state(), cTrue);
    } else {
      rb_raise(rb_eArgError, "utc_offset out of range");
    }

    return MemoryHandle::value(time);
  }
}
