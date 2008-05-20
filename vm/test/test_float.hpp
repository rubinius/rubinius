#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestFloat : public CxxTest::TestSuite {
  public:

#undef state
  VM *state;
  double TOLERANCE;
  void setUp() {
    state = new VM(1024);
    TOLERANCE = 0.00003;
  }

  void tearDown() {
    delete state;
  }

  void check_float(Float* f, Float* g) {
    TS_ASSERT_RELATION(std::greater<double>, f->val + TOLERANCE, g->val);
    TS_ASSERT_RELATION(std::greater<double>, f->val, g->val - TOLERANCE);
    TS_ASSERT_RELATION(std::greater<double>, g->val + TOLERANCE, f->val);
    TS_ASSERT_RELATION(std::greater<double>, g->val, f->val - TOLERANCE);
  }

  void test_create() {
    Float* flt = Float::create(state, 1.0);
    TS_ASSERT_EQUALS(flt->val, 1.0);
  }
  
  void test_compare() {
    Float* fltBig = Float::create(state, 4.0);
    Float* fltSmall = Float::create(state, 9.0);
    TS_ASSERT_EQUALS(fltBig->compare(state, fltSmall), Object::i2n(-1));
    TS_ASSERT_EQUALS(fltSmall->compare(state, fltBig), Object::i2n(1));
    TS_ASSERT_EQUALS(fltSmall->compare(state, fltSmall), Object::i2n(0));
  }
  
  void test_coerce() {
    OBJECT o = Object::i2n(5432);
    Float* coerced = Float::coerce(state, o);
    TS_ASSERT(kind_of<Float>(coerced));
    
    Bignum* bn = Bignum::create(state, (native_int)2147483647);
    Float* coercedBn = Float::coerce(state, bn);
    TS_ASSERT(kind_of<Float>(coercedBn));
    
    String* str = String::create(state, "blah");
    Float* coercedStr = Float::coerce(state, str);
    TS_ASSERT(kind_of<Float>(coercedStr));
    TS_ASSERT_EQUALS(coercedStr->val, 0.0);
  }

  void test_add() {
    Float* f = Float::create(state, 0.2);
    Float* a = f->add(state, Float::create(state, 0.4));
    check_float(a, Float::create(state, 0.6));

    f = Float::create(state, 0.2);
    a = f->add(state, Object::i2n(10));
    check_float(a, Float::create(state, 10.2));

    f = Float::create(state, 0.2);
    a = f->add(state, Bignum::create(state, FIXNUM_MAX + 10));
    check_float(a, Float::create(state, (double)FIXNUM_MAX + 10.2));
  }

  void test_sub() {
    Float* f = Float::create(state, 0.2);
    Float* a = f->sub(state, Float::create(state, 0.4));
    check_float(a, Float::create(state, -0.2));

    f = Float::create(state, 0.2);
    a = f->sub(state, Object::i2n(10));
    check_float(a, Float::create(state, -9.8));

    f = Float::create(state, 0.2);
    a = f->sub(state, Bignum::create(state, FIXNUM_MAX + 10));
    check_float(a, Float::create(state, (double)-FIXNUM_MAX - 9.8));
  }

  void test_mul() {
    Float* f = Float::create(state, 0.2);
    Float* a = f->mul(state, Float::create(state, 0.4));
    check_float(a, Float::create(state, 0.08));

    f = Float::create(state, 0.2);
    a = f->mul(state, Object::i2n(10));
    check_float(a, Float::create(state, 2.0));

    f = Float::create(state, 0.2);
    a = f->mul(state, Bignum::create(state, FIXNUM_MAX + 10));
    check_float(a, Float::create(state, ((double)FIXNUM_MAX + 10) * 0.2));
  }

  void test_divide() {
    Float* f = Float::create(state, 0.2);
    Float* a = f->divide(state, Float::create(state, 0.4));
    check_float(a, Float::create(state, 0.5));

    f = Float::create(state, 0.2);
    a = f->divide(state, Object::i2n(10));
    check_float(a, Float::create(state, 0.02));

    f = Float::create(state, 100000);
    a = f->divide(state, Bignum::create(state, FIXNUM_MAX + 10));
    check_float(a, Float::create(state, 100000 / ((double)FIXNUM_MAX + 10)));
  }

  void test_neg() {
    Float* f = Float::create(state, 0.2);
    Float* a = f->neg(state);
    check_float(a, Float::create(state, -0.2));
  }

  void test_into_string() {
      Float* flt = Float::create(state, 1.0);
      char buf[10];
      flt->into_string(state, buf,10);
      TS_ASSERT(strcmp(buf, "+1.000000") == 0);
  }
};
