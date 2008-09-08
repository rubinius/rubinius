#include "builtin/class.hpp"
#include "builtin/time.hpp"
#include "object_types.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestTime : public CxxTest::TestSuite {
  public:

  VM *state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_create() {
    Time* tm = Time::create(state);

    TS_ASSERT_EQUALS(tm->timeval->obj_type, ArrayType);
    TS_ASSERT_EQUALS(tm->tm->obj_type, ArrayType);
    TS_ASSERT_EQUALS(tm->is_gmt, Qfalse);

    TS_ASSERT_EQUALS(tm->timeval->size(), 2U);
    TS_ASSERT_EQUALS(tm->tm->size(), 11U);
  }

  void test_gettimeofday() {
    Time* tm = Time::create(state);

    tm->timeval = Array::create(state, 0);
    TS_ASSERT_EQUALS(tm->timeval->size(), 0U);

    tm->gettimeofday(state);
    TS_ASSERT_EQUALS(tm->timeval->obj_type, ArrayType);
    TS_ASSERT_EQUALS(tm->timeval->size(), 2U);
  }

  void test_time_switch() {
    Time* tm = Time::create(state);

    tm->tm = Array::create(state, 0);
    TS_ASSERT_EQUALS(tm->tm->size(), 0U);

    tm->is_gmt = Qnil;
    TS_ASSERT_EQUALS(tm->is_gmt, Qnil);

    tm->time_switch(state, Qtrue);
    TS_ASSERT_EQUALS(tm->is_gmt, Qtrue);
    TS_ASSERT_EQUALS(tm->tm->size(), 11U);

    tm->time_switch(state, Qfalse);
    TS_ASSERT_EQUALS(tm->is_gmt, Qfalse);
    TS_ASSERT_EQUALS(tm->tm->size(), 11U);
  }

  void test_time_mktime() {
    FIXNUM sec = Fixnum::from(8);
    FIXNUM min = Fixnum::from(10);
    FIXNUM hour = Fixnum::from(12);
    FIXNUM day = Fixnum::from(18);
    FIXNUM mon = Fixnum::from(10);
    FIXNUM year = Fixnum::from(1973);
    FIXNUM isdst = Fixnum::from(1);
    FIXNUM usec = Fixnum::from(10);
    Integer* tm = Integer::from(state, 119819408);

    Array* ary = Time::mktime(state, sec, min, hour, day, mon, year, usec, isdst, Qfalse);

    TS_ASSERT_EQUALS((unsigned)2, ary->size());
    TS_ASSERT(tm->equal(state, (Integer*)ary->get(state, 0)));
    TS_ASSERT_EQUALS(usec, (FIXNUM)ary->get(state, 1));

    ary = Time::mktime(state, sec, min, hour, day, mon, year, usec, isdst, Qtrue);

    TS_ASSERT_EQUALS((unsigned)2, ary->size());
    TS_ASSERT(tm->equal(state, (Integer*)ary->get(state, 0)));
    TS_ASSERT_EQUALS(usec, (FIXNUM)ary->get(state, 1));
  }

  void test_time_strftime() {
    FIXNUM sec = Fixnum::from(8);
    FIXNUM min = Fixnum::from(10);
    FIXNUM hour = Fixnum::from(12);
    FIXNUM day = Fixnum::from(18);
    FIXNUM mon = Fixnum::from(9);
    FIXNUM year = Fixnum::from(74);
    FIXNUM isdst = Fixnum::from(1);

    Array* ary = Array::create(state, 11);
    ary->set(state, 0, sec);
    ary->set(state, 1, min);
    ary->set(state, 2, hour);
    ary->set(state, 3, day);
    ary->set(state, 4, mon);
    ary->set(state, 5, year);
    ary->set(state, 6, Fixnum::from(0));
    ary->set(state, 7, Fixnum::from(0));
    ary->set(state, 8, isdst);
    ary->set(state, 9, Fixnum::from(-7));
    ary->set(state, 10, String::create(state, ""));

    String* format = String::create(state, "%A, %e %B %G %I:%M:%S %p");
    String* expected = String::create(state, "Sunday, 18 October 1973 12:10:08 PM");
    Time* t = Time::create(state);
    String* actual = t->strftime(state, ary, format);
    TS_ASSERT(actual->equal(state, expected));
  }
};
