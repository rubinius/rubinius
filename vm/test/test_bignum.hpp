#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestBignum : public CxxTest::TestSuite {
  public:

#undef state
  VM* state;
  Bignum* b1;
  Bignum* b2;
  FIXNUM two;

  void setUp() {
    state = new VM(1024);
    b1 = Bignum::create(state, (native_int)2147483647);
    b2 = Bignum::create(state, (native_int)2147483646);
    two = Object::i2n(2);
  }

  void tearDown() {
    delete state;
  }

  void test_create() {
    Bignum* obj = Bignum::create(state, 13);
    TS_ASSERT_EQUALS(obj->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(obj->obj_type, BignumType);
  }

  void test_to_i() {
    Bignum* obj = Bignum::create(state, 13);
    TS_ASSERT_EQUALS(obj->to_i(state), 13);
  }

  void test_create_negative() {
    Bignum* obj = Bignum::create(state, -13);
    TS_ASSERT_EQUALS(obj->to_i(state), -13);
  }

  void test_new_unsigned() {
    Bignum* obj = Bignum::new_unsigned(state, 13);
    TS_ASSERT_EQUALS(obj->to_i(state), 13);
  }

  void test_normalize() {
    Bignum* obj = Bignum::create(state, 13);
    OBJECT out = Bignum::normalize(state, obj);

    TS_ASSERT(out->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(out)->n2i(), 13);

    TS_ASSERT_EQUALS(b1, Bignum::normalize(state, b1));
  }

  void check_bignum(OBJECT big, char* val) {
    char buf[1024];
    std::stringstream stream;

    if(!big->fixnum_p()) {
      ((Bignum*)big)->into_string(state, 10, buf, 1024);
      stream << "bignum not correct: " << buf << " != " << val;
      TSM_ASSERT_EQUALS(stream.str().c_str(), strcmp(val, buf), 0);
    } else {
      std::stringstream str2;
      str2 << as<Integer>(big)->n2i();
      stream << "bignum not correct: " << str2.str() << " != " << val;
      TSM_ASSERT_EQUALS(stream.str().c_str(), strcmp(val, str2.str().c_str()), 0);
    }
  }

  void test_add() {
    check_bignum(b1->add(state, b1), "4294967294");
    check_bignum(b1->add(state, Object::i2n(1)), "2147483648");
  }

  void test_sub() {
    check_bignum(b1->sub(state, b1), "0");
    check_bignum(b1->sub(state, Object::i2n(1)), "2147483646");
  }
  
  void test_mul() {
    check_bignum(b1->mul(state, b1), "4611686014132420609");
    check_bignum(b1->mul(state, Object::i2n(2)), "4294967294");
    check_bignum(b1->mul(state, Object::i2n(1)), "2147483647");
  }

  void test_equal() {
    TS_ASSERT_EQUALS(b1->equal(state, b1), Qtrue);
    TS_ASSERT_EQUALS(b1->equal(state, b2), Qfalse);

    Bignum* b = Bignum::create(state, 2);
    TS_ASSERT_EQUALS(b->equal(state, Object::i2n(2)), Qtrue);
  }

  void test_compare() {
    TS_ASSERT_EQUALS(b1->compare(state, b2), Object::i2n(1));
    TS_ASSERT_EQUALS(b2->compare(state, b1), Object::i2n(-1));
    TS_ASSERT_EQUALS(b1->compare(state, b1), Object::i2n(0));

    TS_ASSERT_EQUALS(b1->compare(state, Object::i2n(0)), Object::i2n(1));
  }

  void test_gt() {
    TS_ASSERT_EQUALS(b1->gt(state, b2), Qtrue);
    TS_ASSERT_EQUALS(b2->gt(state, b1), Qfalse);
    TS_ASSERT_EQUALS(b1->gt(state, b1), Qfalse);
    TS_ASSERT_EQUALS(b1->gt(state, two), Qtrue);
  }
  
  void test_ge() {
    TS_ASSERT_EQUALS(b1->ge(state, b2), Qtrue);
    TS_ASSERT_EQUALS(b2->ge(state, b1), Qfalse);
    TS_ASSERT_EQUALS(b1->ge(state, b1), Qtrue);
    TS_ASSERT_EQUALS(b1->ge(state, two), Qtrue);
  }
  
  void test_lt() {
    TS_ASSERT_EQUALS(b1->lt(state, b2), Qfalse);
    TS_ASSERT_EQUALS(b2->lt(state, b1), Qtrue);
    TS_ASSERT_EQUALS(b1->lt(state, b1), Qfalse);
    TS_ASSERT_EQUALS(b1->lt(state, two), Qfalse);
  }
  
  void test_le() {
    TS_ASSERT_EQUALS(b1->le(state, b2), Qfalse);
    TS_ASSERT_EQUALS(b2->le(state, b1), Qtrue);
    TS_ASSERT_EQUALS(b1->le(state, b1), Qtrue);
    TS_ASSERT_EQUALS(b1->le(state, two), Qfalse);
  }

  void test_from_string_detect() {
    OBJECT b = Bignum::from_string_detect(state, "0x47");
    TS_ASSERT(b->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(b)->n2i(), 0x47);
  }

  void test_from_string() {
    OBJECT b = Bignum::from_string(state, "47", 10);
    TS_ASSERT(b->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(b)->n2i(), 47);
  }

  void test_size() {
    OBJECT s = b1->size(state);
    TS_ASSERT(s->fixnum_p());
    TS_ASSERT(as<Integer>(s)->n2i() > 0);
  }

  void test_hash_bignum() {
    hashval h = b1->hash_bignum(state);
    TS_ASSERT(h > 0);
  }
};
