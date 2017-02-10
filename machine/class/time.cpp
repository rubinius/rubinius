#include "memory.hpp"
#include "object_utils.hpp"

#include "class/array.hpp"
#include "class/class.hpp"
#include "class/exception.hpp"
#include "class/integer.hpp"
#include "class/string.hpp"
#include "class/time.hpp"

#include "util/time64.h"
#include "util/strftime.h"

namespace rubinius {
  void Time::bootstrap(STATE) {
    GO(time_class).set(state->memory()->new_class<Class, Time>(state, "Time"));
  }

  Time* Time::now(STATE, Object* self) {
    Time* tm = state->memory()->new_object<Time>(state, as<Class>(self));

#ifdef HAVE_CLOCK_GETTIME
    struct timespec ts;

    ::clock_gettime(CLOCK_REALTIME, &ts);

    tm->sec(ts.tv_sec);
    tm->nsec(ts.tv_nsec);
#else
    struct timeval tv;

    /* don't fill in the 2nd argument here. getting the timezone here
     * this way is not portable and broken anyway.
     */
    ::gettimeofday(&tv, NULL);

    tm->sec(tv.tv_sec);
    tm->nsec(tv.tv_usec * 1000);
#endif

    return tm;
  }

  Time* Time::at(STATE, time64_t seconds, long nanoseconds) {
    Time* tm = state->memory()->new_object<Time>(state, G(time_class));

    tm->sec(seconds);
    tm->nsec(nanoseconds);

    return tm;
  }

  // Taken from MRI
#define NDIV(x,y) (-(-((x)+1)/(y))-1)
#define NMOD(x,y) ((y)-(-((x)+1)%(y))-1)

  Time* Time::specific(STATE, Object* self, Integer* sec, Integer* nsec,
      Object* gmt, Object* offset)
  {
    Time* tm = state->memory()->new_object<Time>(state, as<Class>(self));

    tm->sec(sec->to_long_long());
    tm->nsec(nsec->to_native());

    // Do a little overflow cleanup.
    if(tm->nsec() >= 1000000000) {
      tm->_sec_ += tm->nsec() / 1000000000;
      tm->_nsec_ %= 1000000000;
    }

    if(tm->nsec() < 0) {
      tm->_sec_ += NDIV(tm->nsec(), 1000000000);
      tm->_nsec_ = NMOD(tm->nsec(), 1000000000);
    }

    tm->is_gmt(state, RBOOL(CBOOL(gmt)));
    tm->offset(state, offset);

    return tm;
  }

  Time* Time::from_array(STATE, Object* self, Fixnum* sec, Fixnum* min, Fixnum* hour,
      Fixnum* mday, Fixnum* mon, Fixnum* year, Fixnum* nsec, Fixnum* isdst,
      Object* from_gmt, Object* offset, Fixnum* offset_sec, Fixnum* offset_nsec)
  {
    struct tm64 tm;

    tm.tm_sec = sec->to_native();
    if(tm.tm_sec < 0 || tm.tm_sec > 60) {
      Exception::raise_argument_error(state, "sec must be in 0..60");
    }

    tm.tm_min = min->to_native();
    if(tm.tm_min < 0 || tm.tm_min > 60) {
      Exception::raise_argument_error(state, "min must be in 0..60");
    }

    tm.tm_hour = hour->to_native();
    if(tm.tm_hour < 0 || tm.tm_hour > 24) {
      Exception::raise_argument_error(state, "hour must be in 0..24");
    }

    tm.tm_mday = mday->to_native();
    if(tm.tm_mday < 1 || tm.tm_mday > 31) {
      Exception::raise_argument_error(state, "mday must be in 1..31");
    }

    tm.tm_mon = mon->to_native() - 1;
    if(tm.tm_mon < 0 || tm.tm_mon > 11) {
      Exception::raise_argument_error(state, "mon must be in 0..11");
    }

    tm.tm_wday = -1;
    tm.tm_gmtoff = 0;
    tm.tm_zone = 0;
    tm.tm_year = year->to_long_long();

    tm.tm_isdst = isdst->to_native();

    time64_t seconds = -1;

    if(CBOOL(from_gmt) || !offset->nil_p()) {
      printf("from_array, call ::timegm64\n");
      seconds = ::timegm64(&tm);
    } else {
      printf("from_array, call tzset and ::timelocal64\n");
      tzset();
      seconds = ::timelocal64(&tm);
    }

    Time* obj = state->memory()->new_object<Time>(state, as<Class>(self));
    obj->sec(seconds);
    obj->nsec(nsec->to_native());
    obj->is_gmt(state, RBOOL(CBOOL(from_gmt)));

    if(!offset->nil_p()) {
      obj->_sec_ -= offset_sec->to_long_long();
      obj->_nsec_ -= offset_nsec->to_native();

      // Deal with underflow wrapping
      if(obj->nsec() < 0) {
        obj->_sec_ += NDIV(obj->nsec(), 1000000000);
        obj->_nsec_ = NMOD(obj->nsec(), 1000000000);
      }

      obj->offset(state, offset);
    }

    return obj;
  }

  Time* Time::dup(STATE, Object* self, Time* other) {
    Time* tm = state->memory()->new_object<Time>(state, as<Class>(self));

    tm->sec(other->sec());
    tm->nsec(other->nsec());
    tm->decomposed(state, other->decomposed());
    tm->is_gmt(state, other->is_gmt());
    tm->offset(state, other->offset());
    tm->zone(state, other->zone());

    return tm;
  }

  void Time::print_tm64(struct tm64* tm) {
#if defined(__FreeBSD__) 
    printf("Time tm64: sec [%d] min [%d] hour [%d] mday [%d] mon [%d] wday [%d] yday [%d] isdst [%d] gmtoff [%d] zone [%s] year [%ld]\n",
    tm->tm_sec,
    tm->tm_min,
    tm->tm_hour,
    tm->tm_mday,
    tm->tm_mon,
    tm->tm_wday,
    tm->tm_yday,
    tm->tm_isdst,
    tm->tm_gmtoff,
    tm->tm_zone,
    tm->tm_year
    );
#else
    printf("Time tm64: sec [%d] min [%d] hour [%d] mday [%d] mon [%d] wday [%d] yday [%d] isdst [%d] gmtoff [%d] zone [%s] year [%lld]\n",
    tm->tm_sec,
    tm->tm_min,
    tm->tm_hour,
    tm->tm_mday,
    tm->tm_mon,
    tm->tm_wday,
    tm->tm_yday,
    tm->tm_isdst,
    tm->tm_gmtoff,
    tm->tm_zone,
    tm->tm_year
    );
#endif
  }

  struct tm64 Time::get_tm() {
    time64_t seconds = sec();
    struct tm64 tm;

    if(Fixnum* off = try_as<Fixnum>(offset())) {
#if defined(__FreeBSD__) 
      printf("Time::get_tm, add offset [%lld] to seconds [%ld]\n", off->to_long_long(), seconds);
#else
      printf("Time::get_tm, add offset [%lld] to seconds [%lld]\n", off->to_long_long(), seconds);
#endif
      seconds += off->to_long_long();
      gmtime64_r(&seconds, &tm);
#if defined(__FreeBSD__) 
      printf("Time::get_tm, called gmtime64_r with [%ld]\n", seconds);
#else
      printf("Time::get_tm, called gmtime64_r with [%lld]\n", seconds);
#endif
      print_tm64(&tm);
    } else if(gmt_p()) {
#if defined(__FreeBSD__) 
      printf("Time::get_tm, calling gmtime64_r with [%ld]\n", seconds);
#else
      printf("Time::get_tm, calling gmtime64_r with [%lld]\n", seconds);
#endif
      gmtime64_r(&seconds, &tm);
      printf("Time::get_tm, called gmtime64_r\n");
      print_tm64(&tm);
    } else {
      printf("Time::get_tm, calling tzset\n");
      tzset();
#if defined(__FreeBSD__) 
      printf("Time::get_tm, tzname [%s] timezone [%s] daylight [not-defined-on-freebsd]\n", *tzname, getenv("TZ"));
#else
      printf("Time::get_tm, tzname [%s] timezone [%ld] daylight [%d]\n", *tzname, timezone, daylight);
#endif
#if defined(__FreeBSD__) 
      printf("Time::get_tm, calling tzset and localtime64_r with [%ld]\n", seconds);
#else
      printf("Time::get_tm, calling tzset and localtime64_r with [%lld]\n", seconds);
#endif
      localtime64_r(&seconds, &tm);
      printf("Time::get_tm, called localtime64_r, tzname [%s] ENV['TZ']=[%s]\n", *tzname, getenv("TZ"));
      print_tm64(&tm);
    }

    return tm;
  }

  Object* Time::utc_offset(STATE) {
    if(gmt_p()) {
      printf("utc_offset, is_gmt is true, returning 0\n");
      return Fixnum::from(0);
    } else if(!offset()->nil_p()) {
      printf("utc_offset, is_gmt is false and offset is non-nil, returning offset [%ld]\n", ((Fixnum*)offset())->to_native());
      return offset();
    }

    native_int off;

#ifdef HAVE_TM_NAME
    struct tm tm = get_tm();
    off = -tm.tm_tzadj;
    printf("utc_offset, HAVE_TM_NAME\n");
#else /* !HAVE_TM_NAME */
#ifdef HAVE_TM_ZONE
#ifdef HAVE_TM_GMTOFF
    struct tm64 tm = get_tm();
    off = tm.tm_gmtoff;
    printf("utc_offset, HAVE_TM_ZONE && HAVE_TM_GMTOFF\n");
#else
    off = _timezone;
    printf("utc_offset, HAVE_TM_ZONE && !HAVE_TM_NAME\n");
#endif
#else /* !HAVE_TM_ZONE */
#if HAVE_VAR_TIMEZONE
#if HAVE_VAR_ALTZONE
    off = -(daylight ? timezone : altzone);
    printf("utc_offset, !HAVE_TM_ZONE && HAVE_VAR_TIMEZONE && HAVE_VAR_ALTZONE\n");
#else
    off = -timezone;
    printf("utc_offset, !HAVE_TM_ZONE && HAVE_VAR_TIMEZONE && !HAVE_VAR_ALTZONE\n");
#endif
#else /* !HAVE_VAR_TIMEZONE */
#ifdef HAVE_GETTIMEOFDAY
    gettimeofday(&tv, &zone);
    off = -zone.tz_minuteswest * 60;
    printf("utc_offset, !HAVE_VAR_TIMEZONE\n");
#else
    /* no timezone info, then calc by myself */
    printf("utc_offset, calc ourselves\n");
    {
      struct tm utc;
      time_t now;
      time(&now);
      gmtime_r(&now, &utc);
      off = (now - mktime(&utc));
    }
#endif
#endif /* !HAVE_VAR_TIMEZONE */
#endif /* !HAVE_TM_ZONE */
#endif /* !HAVE_TM_NAME */
    printf("utc_offset, returning [%ld]\n", off);

    return Fixnum::from(off);
  }

  Array* Time::calculate_decompose(STATE) {
    if(!decomposed()->nil_p()) return decomposed();

    struct tm64 tm = get_tm();

    /* update Time::TM_FIELDS when changing order of fields */
    Array* ary = Array::create(state, 11);
    ary->set(state, 0, Integer::from(state, tm.tm_sec));
    ary->set(state, 1, Integer::from(state, tm.tm_min));
    ary->set(state, 2, Integer::from(state, tm.tm_hour));
    ary->set(state, 3, Integer::from(state, tm.tm_mday));
    ary->set(state, 4, Integer::from(state, tm.tm_mon + 1));
    ary->set(state, 5, Integer::from(state, tm.tm_year));
    ary->set(state, 6, Integer::from(state, tm.tm_wday));
    ary->set(state, 7, Integer::from(state, tm.tm_yday + 1));
    ary->set(state, 8, RBOOL(tm.tm_isdst));

    if (zone()->nil_p()) {
      if(offset()->nil_p() && tm.tm_zone) {
        zone(state, locale_string(state, tm.tm_zone));
      } else {
        zone(state, nil<String>());
      }
    }
    ary->set(state, 9, zone());

    // Cache it.
    decomposed(state, ary);
    return ary;
  }

#define STRFTIME_STACK_BUF 128

  String* Time::strftime(STATE, String* format) {
    struct tm64 tm = get_tm();

    struct timespec64 ts;
    ts.tv_sec = sec();
    ts.tv_nsec = nsec();

    int off = 0;
    if(Fixnum* offset = try_as<Fixnum>(utc_offset(state))) {
      off = offset->to_int();
    }

    if(format->byte_size() == 0) return String::create(state, NULL, 0);

    char stack_str[STRFTIME_STACK_BUF];

    size_t chars = ::strftime_extended(stack_str, STRFTIME_STACK_BUF,
                       format->c_str(state), &tm, &ts, gmt_p() ? 1 : 0,
                       off);

    size_t buf_size = format->byte_size();

    String* result = 0;

    if(chars == 0) {
      buf_size *= 2;
      char* malloc_str = (char*)malloc(buf_size);

      if(!malloc_str) {
        Exception::raise_memory_error(state);
        return NULL;
      }

      chars = ::strftime_extended(malloc_str, buf_size,
                  format->c_str(state), &tm, &ts, gmt_p() ? 1 : 0,
                  off);
      if(chars) {
        result = String::create(state, malloc_str, chars);
        result->encoding(state, format->encoding());
      }

      free(malloc_str);
    } else {
      result = String::create(state, stack_str, chars);
      result->encoding(state, format->encoding());
    }

    return result;
  }

  // Returns the environment time zone.
  String* Time::env_zone(STATE) {
    struct tm64 tm;

    tzset();
    localtime64_r(&_sec_, &tm);

    if(tm.tm_zone) {
      return locale_string(state, tm.tm_zone);
    } else {
      return nil<String>();
    }
  }

  String* Time::locale_string(STATE, const char* data) {
    String* str = String::create(state, data);
    Encoding* locale = Encoding::find(state, "locale");
    if(!locale->nil_p()) {
      str->encoding(state, locale);
    }
    return str;
  }
}
