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
};
