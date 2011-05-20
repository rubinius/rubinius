#include "vm.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"
#include "primitives.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/exception.hpp"
#include "builtin/integer.hpp"
#include "builtin/string.hpp"
#include "builtin/time.hpp"

#include "util/time.h"

#include <sys/time.h>
#include <time.h>

namespace rubinius {
  void Time::init(STATE) {
    GO(time_class).set(state->new_class("Time", G(object)));
    G(time_class)->set_object_type(state, TimeType);
  }

  Time* Time::now(STATE, Object* self) {
    struct timeval tv;

    /* don't fill in the 2nd argument here. getting the timezone here
     * this way is not portable and broken anyway.
     */
    ::gettimeofday(&tv, NULL);

    Time* tm = state->new_object<Time>(as<Class>(self));

    tm->seconds_ = tv.tv_sec;
    tm->microseconds_ = tv.tv_usec;

    tm->is_gmt(state, Qfalse);

    return tm;
  }

  // Taken from MRI
#define NDIV(x,y) (-(-((x)+1)/(y))-1)
#define NMOD(x,y) ((y)-(-((x)+1)%(y))-1)

  Time* Time::specific(STATE, Object* self, Integer* sec, Integer* usec,
                       Object* gmt)
  {
    Time* tm = state->new_object<Time>(as<Class>(self));

    if(sizeof(time_t) == sizeof(long long)) {
      tm->seconds_ = sec->to_long_long();
      tm->microseconds_ = usec->to_long_long();
    } else {
      tm->seconds_ = sec->to_native();
      tm->microseconds_ = usec->to_native();
    }

    // Do a little overflow cleanup.
    if(tm->microseconds_ >= 1000000) {
      tm->seconds_ += tm->microseconds_ / 1000000;
      tm->microseconds_ %= 1000000;
    }

    if(tm->microseconds_ < 0) {
      tm->seconds_ += NDIV(tm->microseconds_,1000000);
      tm->microseconds_ = NMOD(tm->microseconds_, 1000000);
    }

    tm->is_gmt(state, RTEST(gmt) ? Qtrue : Qfalse);

    return tm;
  }

  Time* Time::from_array(STATE, Object* self, 
                      Fixnum* sec, Fixnum* min, Fixnum* hour,
                      Fixnum* mday, Fixnum* mon, Fixnum* year, Fixnum* usec,
                      Fixnum* isdst, Object* from_gmt) {
    struct tm tm;

    tm.tm_sec = sec->to_native();
    if(tm.tm_sec < 0 || tm.tm_sec > 60) {
      Exception::argument_error(state, "sec must be in 0..60");
    }

    tm.tm_min = min->to_native();
    if(tm.tm_min < 0 || tm.tm_min > 60) {
      Exception::argument_error(state, "min must be in 0..60");
    }

    tm.tm_hour = hour->to_native();
    if(tm.tm_hour < 0 || tm.tm_hour > 24) {
      Exception::argument_error(state, "hour must be in 0..24");
    }

    tm.tm_mday = mday->to_native();
    if(tm.tm_mday < 1 || tm.tm_mday > 31) {
      Exception::argument_error(state, "mday must be in 1..31");
    }

    tm.tm_mon = mon->to_native() - 1;
    if(tm.tm_mon < 0 || tm.tm_mon > 11) {
      Exception::argument_error(state, "mon must be in 0..11");
    }

    tm.tm_wday = -1;
    tm.tm_gmtoff = 0;
    tm.tm_zone = 0;
    tm.tm_year = year->to_native() - 1900;

    tm.tm_isdst = isdst->to_native();

    time_t seconds = -1;

    if(RTEST(from_gmt)) {
      seconds = ::timegm(&tm);
    } else {
      tzset();
      seconds = ::mktime(&tm);
    }

    int err = 0;

    if(seconds == -1) {
      int utc_p = from_gmt->true_p() ? 1 : 0;
      seconds = mktime_extended(&tm, utc_p, &err);
    }

    if(err) return (Time*)Primitives::failure();

    Time* obj = state->new_object<Time>(as<Class>(self));
    obj->seconds_ = seconds;
    obj->microseconds_ = usec->to_native();
    obj->is_gmt(state, RTEST(from_gmt) ? Qtrue : Qfalse);

    return obj;
  }

  Time* Time::dup(STATE) {
    Time* tm = state->new_object<Time>(class_object(state));
    tm->seconds_ = seconds_;
    tm->microseconds_ = microseconds_;
    tm->is_gmt(state, is_gmt_);
    return tm;
  }

  Array* Time::calculate_decompose(STATE, Object* use_gmt) {
    if(!decomposed_->nil_p()) return decomposed_;

    time_t seconds = seconds_;
    struct tm tm;

    if(RTEST(use_gmt)) {
      gmtime_r(&seconds, &tm);
    } else {
      tzset();
      localtime_r(&seconds, &tm);
    }

    /* update Time::TM_FIELDS when changing order of fields */
    Array* ary = Array::create(state, 11);
    ary->set(state, 0, Integer::from(state, tm.tm_sec));
    ary->set(state, 1, Integer::from(state, tm.tm_min));
    ary->set(state, 2, Integer::from(state, tm.tm_hour));
    ary->set(state, 3, Integer::from(state, tm.tm_mday));
    ary->set(state, 4, Integer::from(state, tm.tm_mon + 1));
    ary->set(state, 5, Integer::from(state, tm.tm_year + 1900));
    ary->set(state, 6, Integer::from(state, tm.tm_wday));
    ary->set(state, 7, Integer::from(state, tm.tm_yday + 1));
    ary->set(state, 8, tm.tm_isdst ? Qtrue : Qfalse);

#ifdef HAVE_STRUCT_TM_TM_ZONE
    ary->set(state, 9, String::create(state, tm.tm_zone));
#else
    ary->set(state, 9, Qnil);
#endif

    // Cache it.
    decomposed(state, ary);
    return ary;
  }

#define MAX_STRFTIME_OUTPUT 128

  String* Time::strftime(STATE, String* format) {
    struct tm tm;
    char str[MAX_STRFTIME_OUTPUT];
    int is_gmt = 0;

    time_t seconds = seconds_;

    if(RTEST(is_gmt_)) {
      is_gmt = 1;
      gmtime_r(&seconds, &tm);
    } else {
      tzset();
      localtime_r(&seconds, &tm);
    }

    struct timespec ts = { seconds, 0 };

    size_t chars = ::strftime_extended(str, MAX_STRFTIME_OUTPUT,
                       format->c_str(state), &tm, &ts, is_gmt);
    str[MAX_STRFTIME_OUTPUT-1] = 0;

    return String::create(state, str, chars);
  }
}
