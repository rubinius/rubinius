#include "vm.hpp"
#include "objectmemory.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/integer.hpp"
#include "builtin/time.hpp"

#include <sys/time.h>
#include <time.h>

namespace rubinius {
  void Time::init(STATE) {
    GO(time_class).set(state->new_class("Time", G(object), Time::fields));
    G(time_class)->set_object_type(TimeType);
  }

  Time* Time::create(STATE) {
    Time* tm = (Time*)state->om->new_object(G(time_class), Time::fields);

    tm->gettimeofday(state);
    tm->time_switch(state, Qfalse);

    return tm;
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

    SET(this, timeval, ary);

    return this;
  }

  Time* Time::time_switch(STATE, OBJECT gmt) {
    time_t seconds = ((Integer*)this->timeval->get(state, 0))->to_native();
    struct tm *tm;

    if(gmt == Qtrue) {
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

    SET(this, tm, ary);
    SET(this, is_gmt, gmt);

    return this;
  }
}
