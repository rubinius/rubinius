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

  void test_kind_of_numeric() {
    Fixnum* fix = Fixnum::from(13);
    Bignum* big = Bignum::from(state, (native_int)14);
    Float*  flt = Float::create(state, 15.0);
    Object* obj = state->om->new_object(G(object), Object::fields);

    TS_ASSERT(kind_of<Numeric>(fix));
    TS_ASSERT(kind_of<Numeric>(big));
    TS_ASSERT(kind_of<Numeric>(flt));
    TS_ASSERT(!kind_of<Numeric>(obj));
    TS_ASSERT(!kind_of<Numeric>(Qtrue));
    TS_ASSERT(!kind_of<Numeric>(Qfalse));
    TS_ASSERT(!kind_of<Numeric>(Qnil));
  }

  void test_from_native_int() {
    Integer* obj = Integer::from(state, (native_int)13);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int) 13);

    obj = Integer::from(state, (native_int)-13);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int)-13);

    obj = Integer::from(state, (native_int)2147483647);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int)2147483647);
  }

  void test_from_int() {
    Integer* obj = Integer::from(state, (int)13);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int) 13);

    obj = Integer::from(state, (int)-13);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int)-13);

    obj = Integer::from(state, (int)2147483647);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int)2147483647);
  }

  void test_from_unsigned_int() {
    Integer* obj = Integer::from(state, (unsigned int)13);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int) 13);

    obj = Integer::from(state, (unsigned int)2147483647);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int)2147483647);
  }

  void test_from_unsigned_long() {
    Integer* obj = Integer::from(state, (unsigned long)13);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int) 13);

    obj = Integer::from(state, (unsigned long)2147483647);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int)2147483647);
  }

  void test_from_long_long() {
    Integer* obj = Integer::from(state, (long long)13);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int) 13);

    obj = Integer::from(state, (long long)-13);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int)-13);

    obj = Integer::from(state, 214748364712331LL);
    TS_ASSERT_EQUALS(as<Bignum>(obj)->to_long_long(), 214748364712331LL);
  }

  void test_from_unsigned_long_long() {
    Integer* obj = Integer::from(state, (unsigned long long)13);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int) 13);

    obj = Integer::from(state, (unsigned long long)-13);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int)-13);

    obj = Integer::from(state, 214748364712331ULL);
    TS_ASSERT_EQUALS(as<Bignum>(obj)->to_ulong_long(), 214748364712331ULL);
  }
};
