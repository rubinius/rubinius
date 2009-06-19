#include "vm/test/test.hpp"

#include "builtin/class.hpp"
#include "builtin/time.hpp"
#include "object_types.hpp"

class TestTime : public CxxTest::TestSuite, public VMTest {
public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_create() {
    Time* tm = Time::create(state);

    TS_ASSERT(kind_of<Integer>(tm->sec()));
    TS_ASSERT(kind_of<Integer>(tm->usec()));
    TS_ASSERT_EQUALS(tm->is_gmt(), Qfalse);
  }

  void test_gettimeofday() {
    Time* tm = Time::create(state);

    tm->gettimeofday(state);

    TS_ASSERT(tm->sec()->to_native() > 0);
  }

  void test_time_switch() {
    Time* tm = Time::create(state);

    tm->tm(state, Array::create(state, 0));
    TS_ASSERT_EQUALS(tm->tm()->size(), 0U);

    tm->is_gmt(state, Qnil);
    TS_ASSERT_EQUALS(tm->is_gmt(), Qnil);

    tm->time_switch(state, Qtrue);
    TS_ASSERT_EQUALS(tm->is_gmt(), Qtrue);
    TS_ASSERT_EQUALS(tm->tm()->size(), 11U);

    tm->time_switch(state, Qfalse);
    TS_ASSERT_EQUALS(tm->is_gmt(), Qfalse);
    TS_ASSERT_EQUALS(tm->tm()->size(), 11U);
  }

  void test_time_mktime() {
    Fixnum* sec = Fixnum::from(8);
    Fixnum* min = Fixnum::from(10);
    Fixnum* hour = Fixnum::from(12);
    Fixnum* day = Fixnum::from(18);
    Fixnum* mon = Fixnum::from(10);
    Fixnum* year = Fixnum::from(1973);
    Fixnum* isdst = Fixnum::from(1);
    Fixnum* usec = Fixnum::from(10);
    Integer* tm = Integer::from(state, 119819408);

    Tuple* tup = Time::mktime(state, sec, min, hour, day, mon, year, usec, isdst, Qfalse);

    TS_ASSERT_EQUALS((unsigned)2, tup->num_fields());
    TS_ASSERT(tm->equal(state, (Integer*)tup->at(state, 0)));
    TS_ASSERT_EQUALS(usec, (Fixnum*)tup->at(state, 1));

    tup = Time::mktime(state, sec, min, hour, day, mon, year, usec, isdst, Qtrue);

    TS_ASSERT_EQUALS((unsigned)2, tup->num_fields());
    TS_ASSERT(tm->equal(state, (Integer*)tup->at(state, 0)));
    TS_ASSERT_EQUALS(usec, (Fixnum*)tup->at(state, 1));
  }

  void test_time_strftime() {
    Fixnum* sec = Fixnum::from(8);
    Fixnum* min = Fixnum::from(10);
    Fixnum* hour = Fixnum::from(12);
    Fixnum* day = Fixnum::from(18);
    Fixnum* mon = Fixnum::from(9);
    Fixnum* year = Fixnum::from(74);
    Fixnum* isdst = Fixnum::from(1);

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
    TS_ASSERT_EQUALS(actual->equal(state, expected), Qtrue);
  }
};
