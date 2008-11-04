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

#include <sys/time.h>
#include <time.h>

namespace rubinius {
  void Time::init(STATE) {
    GO(time_class).set(state->new_class("Time", G(object), Time::fields));
    G(time_class)->set_object_type(state, TimeType);
  }

  Time* Time::create(STATE) {
    Time* tm = (Time*)state->om->new_object(G(time_class), Time::fields);

    tm->gettimeofday(state);
    tm->time_switch(state, Qfalse);

    return tm;
  }

  Time* Time::initialize_copy(STATE, Time* other) {
    this->timeval(state, other->timeval_);
    this->tm(state, other->tm_);
    this->is_gmt(state, other->is_gmt_);
    return this;
  }

  Time* Time::gettimeofday(STATE) {
    struct timeval tv;

    /* don't fill in the 2nd argument here. getting the timezone here
     * this way is not portable and broken anyway.
     */
    ::gettimeofday(&tv, NULL);

    /* update Time::TIMEVAL_FIELDS when changing order of fields */
    Array* ary = Array::create(state, 2);
    ary->set(state, 0, Integer::from(state, tv.tv_sec));
    ary->set(state, 1, Integer::from(state, tv.tv_usec));

    this->timeval(state, ary);

    return this;
  }

  Time* Time::time_switch(STATE, Object* gmt) {
    time_t seconds = ((Integer*)timeval_->get(state, 0))->to_native();
    struct tm *tm;

    if(gmt->true_p()) {
      tm = gmtime(&seconds);
    } else {
      tm = localtime(&seconds);
    }

    /* update Time::TM_FIELDS when changing order of fields */
    Array* ary = Array::create(state, 11);
    ary->set(state, 0, Integer::from(state, tm->tm_sec));
    ary->set(state, 1, Integer::from(state, tm->tm_min));
    ary->set(state, 2, Integer::from(state, tm->tm_hour));
    ary->set(state, 3, Integer::from(state, tm->tm_mday));
    ary->set(state, 4, Integer::from(state, tm->tm_mon));
    ary->set(state, 5, Integer::from(state, tm->tm_year));
    ary->set(state, 6, Integer::from(state, tm->tm_wday));
    ary->set(state, 7, Integer::from(state, tm->tm_yday));
    ary->set(state, 8, Integer::from(state, tm->tm_isdst));

#ifdef HAVE_STRUCT_TM_TM_GMTOFF
    ary->set(state, 9, Integer::from(state, tm->tm_gmtoff));
#else
    ary->set(state, 9, Qnil);
#endif

#ifdef HAVE_STRUCT_TM_TM_ZONE
    ary->set(state, 10, String::create(state, tm->tm_zone));
#else
    ary->set(state, 10, Qnil);
#endif

    this->tm(state, ary);
    this->is_gmt(state, gmt);

    return this;
  }

  Array* Time::mktime(STATE, Fixnum* sec, Fixnum* min, Fixnum* hour,
                     Fixnum* mday, Fixnum* mon, Fixnum* year, Fixnum* usec,
                     Fixnum* isdst, Object* from_gmt) {
    struct tm tm;
    char *old_tz = NULL;
    char old_tz_buf[128];

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

    tm.tm_year = year->to_native() - 1900;

    /* In theory, we'd set the tm_isdst field to isdst->to_native().
     * But since that will break on at least FreeBSD,
     * and I don't see the point of filling in that flag at all,
     * we're telling the system here to figure the DST stuff
     * out itmsg->recv.
     */
    tm.tm_isdst = -1;

    if(from_gmt->true_p()) {
      old_tz = getenv("TZ");

      /* We need to save old_tz to our own buffer here, because e.g.
       * FreeBSD's setenv() will manipulate that string directly.
       */
      if(old_tz) {
        strncpy(old_tz_buf, old_tz, sizeof(old_tz_buf));
        old_tz_buf[sizeof(old_tz_buf) - 1] = 0;
      }

      setenv("TZ", "", 1);
    }

    time_t seconds = ::mktime(&tm);

    if(from_gmt->true_p()) {
      if(old_tz) {
        setenv("TZ", old_tz_buf, 1);
      } else {
        unsetenv("TZ");
      }
    }

    Array* ary = Array::create(state, 2);
    ary->set(state, 0, Integer::from(state, seconds));
    ary->set(state, 1, usec);

    return ary;
  }

#define MAX_STRFTIME_OUTPUT 128

  String* Time::strftime(STATE, Array* ary, String* format) {
    struct tm tm;
    char str[MAX_STRFTIME_OUTPUT];

    tm.tm_sec = ((Fixnum*)ary->get(state, 0))->to_native();
    tm.tm_min = ((Fixnum*)ary->get(state, 1))->to_native();
    tm.tm_hour = ((Fixnum*)ary->get(state, 2))->to_native();
    tm.tm_mday = ((Fixnum*)ary->get(state, 3))->to_native();
    tm.tm_mon = ((Fixnum*)ary->get(state, 4))->to_native();
    tm.tm_year = ((Fixnum*)ary->get(state, 5))->to_native();
    tm.tm_wday = ((Fixnum*)ary->get(state, 6))->to_native();
    tm.tm_yday = ((Fixnum*)ary->get(state, 7))->to_native();
    tm.tm_isdst = ((Fixnum*)ary->get(state, 8))->to_native();

#ifdef HAVE_STRUCT_TM_TM_GMTOFF
    tm.tm_gmtoff = ((Fixnum*)ary->get(state, 9))->to_native();
#endif

#ifdef HAVE_STRUCT_TM_TM_ZONE
    /** @todo There are platform diffs here. The string should be copied
     *        if (and only if) struct tm does not have a const tm_zone,
     *        but for now, just reference the original. It *should* be
     *        safe. --rue */
    tm.tm_zone = const_cast<char*>(as<String>(ary->get(state, 10))->c_str());
#endif

    size_t chars = ::strftime(str, MAX_STRFTIME_OUTPUT,
                              format->c_str(), &tm);
    str[MAX_STRFTIME_OUTPUT-1] = 0;

    return String::create(state, str, chars);
  }
}
