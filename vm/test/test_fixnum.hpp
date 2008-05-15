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
  }

  void test_div() {
    FIXNUM one = as<Fixnum>(Object::i2n(4));

    FIXNUM two = as<Fixnum>(one->div(state, one));
    TS_ASSERT_EQUALS(two->n2i(), 1);
  }

  void test_mod() {
    FIXNUM one = as<Fixnum>(Object::i2n(4));

    FIXNUM two = as<Fixnum>(one->mod(state, one));
    TS_ASSERT_EQUALS(two->n2i(), 0);
  }
};
