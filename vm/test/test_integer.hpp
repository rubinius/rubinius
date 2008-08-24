#include "builtin/list.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestInteger : public CxxTest::TestSuite {
  public:

  VM* state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_from_native_int() {
    INTEGER obj = Integer::from(state, (native_int)13);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int) 13);

    obj = Integer::from(state, (native_int)-13);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int)-13);

    obj = Integer::from(state, (native_int)2147483647);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int)2147483647);
  }

  void test_from_int() {
    INTEGER obj = Integer::from(state, (int)13);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int) 13);

    obj = Integer::from(state, (int)-13);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int)-13);

    obj = Integer::from(state, (int)2147483647);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int)2147483647);
  }

  void test_from_unsigned_int() {
    INTEGER obj = Integer::from(state, (unsigned int)13);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int) 13);

    obj = Integer::from(state, (unsigned int)2147483647);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int)2147483647);
  }

  void test_from_unsigned_long() {
    INTEGER obj = Integer::from(state, (unsigned long)13);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int) 13);

    obj = Integer::from(state, (unsigned long)2147483647);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int)2147483647);
  }

  void test_from_long_long() {
    INTEGER obj = Integer::from(state, (long long)13);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int) 13);

    obj = Integer::from(state, (long long)-13);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int)-13);

    obj = Integer::from(state, 214748364712331LL);
    TS_ASSERT_EQUALS(as<Bignum>(obj)->to_long_long(), 214748364712331LL);
  }

  void test_from_unsigned_long_long() {
    INTEGER obj = Integer::from(state, (unsigned long long)13);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int) 13);

    obj = Integer::from(state, (unsigned long long)-13);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int)-13);

    obj = Integer::from(state, 214748364712331ULL);
    TS_ASSERT_EQUALS(as<Bignum>(obj)->to_ulong_long(), 214748364712331ULL);
  }
};
