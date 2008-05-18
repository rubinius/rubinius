#include "objects.hpp"
#include "builtin_list.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestFixnum : public CxxTest::TestSuite {
  public:

#undef state
  VM* state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_add() {
    FIXNUM one = as<Fixnum>(Object::i2n(1));

    FIXNUM two = as<Fixnum>(one->add(state, one));
    TS_ASSERT_EQUALS(two->n2i(), 2);
  }
  
  void test_add_overflows_to_bignum() {
    FIXNUM  one = as<Fixnum>(Object::i2n(FIXNUM_MAX - 10));
    INTEGER two = as<Integer>(one->add(state, one));

    TS_ASSERT_EQUALS(two->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(two->n2i(), (FIXNUM_MAX - 10) * 2);
  }

  void test_add_underflows_to_bignum() {
    FIXNUM  one = as<Fixnum>(Object::i2n(FIXNUM_MIN + 10));
    INTEGER two = as<Integer>(one->add(state, one));

    TS_ASSERT_EQUALS(two->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(two->n2i(), (FIXNUM_MIN + 10) * 2);
  }

  void test_add_a_bignum() {
    FIXNUM one  = as<Fixnum>(Object::i2n(13));
    Bignum* obj = Bignum::create(state, FIXNUM_MAX - 10);
    INTEGER res = one->add(state, obj);
    TS_ASSERT_EQUALS(res->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(res->n2i(), FIXNUM_MAX + 3);
  }

  void test_sub() {
    FIXNUM one = as<Fixnum>(Object::i2n(1));

    FIXNUM zero = as<Fixnum>(one->sub(state, one));
    TS_ASSERT_EQUALS(zero->n2i(), 0);
  }

  void test_sub_overflows_to_bignum() {
    FIXNUM  one   = as<Fixnum>(Object::i2n(FIXNUM_MIN + 10));
    FIXNUM  two   = as<Fixnum>(Object::i2n(FIXNUM_MAX - 10));
    INTEGER three = as<Integer>(two->sub(state, one));

    TS_ASSERT_EQUALS(three->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(three->n2i(), FIXNUM_MAX - 10 - (FIXNUM_MIN + 10));
  }

  void test_sub_underflows_to_bignum() {
    FIXNUM  one   = as<Fixnum>(Object::i2n(FIXNUM_MIN + 10));
    FIXNUM  two   = as<Fixnum>(Object::i2n(FIXNUM_MAX - 10));
    INTEGER three = as<Integer>(one->sub(state, two));

    TS_ASSERT_EQUALS(three->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(three->n2i(), FIXNUM_MIN + 10 - (FIXNUM_MAX - 10));
  }

  void test_sub_a_bignum() {
    FIXNUM one  = as<Fixnum>(Object::i2n(13));
    Bignum* obj = Bignum::create(state, FIXNUM_MAX + 28);
    INTEGER res = one->sub(state, obj);
    TS_ASSERT_EQUALS(res->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(res->n2i(), 13 - (FIXNUM_MAX + 28));
  }

  void test_mul() {
    FIXNUM one = as<Fixnum>(Object::i2n(4));

    FIXNUM two = as<Fixnum>(one->mul(state, one));
    TS_ASSERT_EQUALS(two->n2i(), 16);
  }

  void test_mul_overflows_to_bignum() {
    FIXNUM  one = as<Fixnum>(Object::i2n(FIXNUM_MAX - 10));
    INTEGER two = as<Integer>(one->mul(state, Object::i2n(2)));

    TS_ASSERT_EQUALS(two->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(two->n2i(), (FIXNUM_MAX - 10) * 2);

    FIXNUM  three = as<Fixnum>(Object::i2n(FIXNUM_MIN + 10));
    INTEGER four  = as<Integer>(three->mul(state, Object::i2n(2)));

    TS_ASSERT_EQUALS(four->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(four->n2i(), (FIXNUM_MIN + 10) * 2);

    FIXNUM  five = as<Fixnum>(Object::i2n(FIXNUM_MAX - 10));
    INTEGER six  = as<Integer>(five->mul(state, Object::i2n(-2)));

    TS_ASSERT_EQUALS(six->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(six->n2i(), (FIXNUM_MAX - 10) * -2);

    FIXNUM  seven = as<Fixnum>(Object::i2n(FIXNUM_MIN + 10));
    INTEGER eight = as<Integer>(seven->mul(state, Object::i2n(-2)));

    TS_ASSERT_EQUALS(eight->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(eight->n2i(), (FIXNUM_MIN + 10) * -2);

  }

  void test_mul_with_bignum() {
    FIXNUM  one   = as<Fixnum>(Object::i2n(2));
    Bignum* two   = Bignum::create(state, FIXNUM_MAX + 10);
    INTEGER three = one->mul(state, two);
    
    TS_ASSERT_EQUALS(three->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(three->n2i(), (FIXNUM_MAX + 10) * 2);
  }

  void test_div() {
    FIXNUM one = as<Fixnum>(Object::i2n(4));

    FIXNUM two = as<Fixnum>(one->div(state, one));
    TS_ASSERT_EQUALS(two->n2i(), 1);
  }

  void test_div_with_positive_arguments() {
    TS_ASSERT_EQUALS(Object::i2n(3)->div(state, Object::i2n(2)), Object::i2n(1));
  }

  void test_div_with_first_negative_argument() {
    TS_ASSERT_EQUALS(Object::i2n(-3)->div(state, Object::i2n(2)), Object::i2n(-2));
  }

  void test_div_with_second_negative_argument() {
    TS_ASSERT_EQUALS(Object::i2n(3)->div(state, Object::i2n(-2)), Object::i2n(-2));
  }

  void test_div_with_negative_arguments() {
    TS_ASSERT_EQUALS(Object::i2n(-3)->div(state, Object::i2n(-2)), Object::i2n(1));
  }

  void test_mod() {
    FIXNUM one = as<Fixnum>(Object::i2n(4));

    FIXNUM two = as<Fixnum>(one->mod(state, one));
    TS_ASSERT_EQUALS(two->n2i(), 0);
  }

  void test_mod_with_positive_arguments() {
    TS_ASSERT_EQUALS(Object::i2n(3)->mod(state, Object::i2n(2)), Object::i2n(1));
  }

  void test_mod_with_first_negative_argument() {
    TS_ASSERT_EQUALS(Object::i2n(-3)->mod(state, Object::i2n(2)), Object::i2n(1));
  }

  void test_mod_with_second_negative_argument() {
    TS_ASSERT_EQUALS(Object::i2n(3)->mod(state, Object::i2n(-2)), Object::i2n(-1));
  }

  void test_mod_with_negative_arguments() {
    TS_ASSERT_EQUALS(Object::i2n(-3)->mod(state, Object::i2n(-2)), Object::i2n(-1));
  }

  void test_divmod() {
    FIXNUM one = as<Fixnum>(Object::i2n(15));
    FIXNUM two = as<Fixnum>(Object::i2n(4));

    Array* ary1 = one->divmod(state, two);
    Object* o1 = ary1->get(state, 0);
    Object* o2 = ary1->get(state, 1);
    TS_ASSERT(o1->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(o1)->n2i(), 3);
    TS_ASSERT(o2->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(o2)->n2i(), 3);
  }
};
