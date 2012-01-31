#include "vm/test/test.hpp"

#include "builtin/list.hpp"

class TestInteger : public CxxTest::TestSuite, public VMTest {
public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_kind_of_numeric() {
    Fixnum* fix = Fixnum::from(13);
    Bignum* big = Bignum::from(state, (native_int)14);
    Float*  flt = Float::create(state, 15.0);
    Object* obj = state->new_object<Object>(G(object));

    TS_ASSERT(kind_of<Numeric>(fix));
    TS_ASSERT(kind_of<Numeric>(big));
    TS_ASSERT(kind_of<Numeric>(flt));
    TS_ASSERT(!kind_of<Numeric>(obj));
    TS_ASSERT(!kind_of<Numeric>(cTrue));
    TS_ASSERT(!kind_of<Numeric>(cFalse));
    TS_ASSERT(!kind_of<Numeric>(cNil));
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

    obj = Integer::from(state, 9223372036854775807LL);
    TS_ASSERT_EQUALS(as<Bignum>(obj)->to_long_long(), 9223372036854775807LL);
  }

  void test_from_unsigned_long_long() {
    Integer* obj = Integer::from(state, (unsigned long long)13);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int) 13);

    obj = Integer::from(state, (unsigned long long)-13);
    TS_ASSERT_EQUALS(obj->to_native(), (native_int)-13);

    obj = Integer::from(state, 9223372036854775808ULL);
    TS_ASSERT_EQUALS(as<Bignum>(obj)->to_ulong_long(), 9223372036854775808ULL);
  }
};
