#include "objects.hpp"
#include "builtin_list.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestFixnum : public CxxTest::TestSuite {
  public:

  VM* state;
  double TOLERANCE;

  void setUp() {
    state = new VM();
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

  void test_add_a_float() {
    FIXNUM one = Object::i2n(13);
    Float* res = one->add(state, Float::create(state, 1.4));
    check_float(res, Float::create(state, 14.4));
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

  void test_sub_a_float() {
    FIXNUM one = Object::i2n(13);
    Float* res = one->sub(state, Float::create(state, 1.4));
    check_float(res, Float::create(state, 11.6));
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

  void test_mul_a_float() {
    FIXNUM one = Object::i2n(13);
    Float* res = one->mul(state, Float::create(state, 1.4));
    check_float(res, Float::create(state, 18.2));
  }

  void test_div() {
    FIXNUM one = as<Fixnum>(Object::i2n(4));

    FIXNUM two = as<Fixnum>(one->div(state, one));
    TS_ASSERT_EQUALS(two->n2i(), 1);

    TS_ASSERT_THROWS( one->div(state, Object::i2n(0)), const ZeroDivisionError &);
  }

  void test_div_a_bignum() {
    FIXNUM one = Object::i2n(13);
    INTEGER res = one->div(state, Bignum::create(state, FIXNUM_MAX + 10));
    TS_ASSERT_EQUALS(res->n2i(), 0);

    TS_ASSERT_THROWS( one->div(state, Bignum::create(state, 0)), const ZeroDivisionError &);
  }

  void test_div_a_float() {
    FIXNUM one = Object::i2n(13);
    Float* res = one->div(state, Float::create(state, 1.4));
    check_float(res, Float::create(state, 9.28571428571));
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

  void test_mod_with_a_bignum() {
    TS_ASSERT_EQUALS(Object::i2n(-3)->mod(state, Bignum::create(state, -2)), Object::i2n(-1));
  }

  void test_mod_with_a_float() {
    Float* f = Object::i2n(4)->mod(state, Float::create(state, -1.7));
    check_float(f, Float::create(state, -1.1));
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

    TS_ASSERT_THROWS( one->divmod(state, Object::i2n(0)), const ZeroDivisionError &);
  }

  void test_divmod_with_a_bignum() {
    FIXNUM one = as<Fixnum>(Object::i2n(15));
    Bignum* two = Bignum::create(state, 4);

    Array* ary1 = one->divmod(state, two);
    Object* o1 = ary1->get(state, 0);
    Object* o2 = ary1->get(state, 1);
    TS_ASSERT(o1->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(o1)->n2i(), 3);
    TS_ASSERT(o2->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(o2)->n2i(), 3);
  }

  void test_divmod_with_a_float() {
    FIXNUM one = as<Fixnum>(Object::i2n(15));

    Array* ary1 = one->divmod(state, Float::create(state, -3.3));
    Object* o1 = ary1->get(state, 0);
    Object* o2 = ary1->get(state, 1);
    TS_ASSERT(o1->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(o1)->n2i(), -5);
    check_float(as<Float>(o2), Float::create(state, -1.5));
  }

  void test_neg() {
    TS_ASSERT_EQUALS(Object::i2n(3)->neg(state),  Object::i2n(-3));
    TS_ASSERT_EQUALS(Object::i2n(-3)->neg(state),  Object::i2n(3));
  }

  void test_equal() {
    TS_ASSERT_EQUALS(Object::i2n(3)->equal(state, Object::i2n(3)), Qtrue);
    TS_ASSERT_EQUALS(Object::i2n(4)->equal(state, Object::i2n(3)), Qfalse);
  }

  void test_equal_with_a_bignum() {
    Bignum* obj1 = Bignum::create(state, 10);
    Bignum* obj2 = Bignum::create(state, 11);
    
    TS_ASSERT_EQUALS(Object::i2n(10)->equal(state, obj1), Qtrue);
    TS_ASSERT_EQUALS(Object::i2n(10)->equal(state, obj2), Qfalse);
  }

  void test_equal_with_a_float() {
    Float* obj1 = Float::create(state, 10.0);
    Float* obj2 = Float::create(state, 10.1);
    
    TS_ASSERT_EQUALS(Object::i2n(10)->equal(state, obj1), Qtrue);
    TS_ASSERT_EQUALS(Object::i2n(10)->equal(state, obj2), Qfalse);
  }

  void test_compare() {
    TS_ASSERT_EQUALS(Object::i2n(3)->compare(state, Object::i2n(2)), Object::i2n(1));
    TS_ASSERT_EQUALS(Object::i2n(3)->compare(state, Object::i2n(3)), Object::i2n(0));
    TS_ASSERT_EQUALS(Object::i2n(3)->compare(state, Object::i2n(4)), Object::i2n(-1));
  }

  void test_compare_with_a_bignum() {
    TS_ASSERT_EQUALS(Object::i2n(3)->compare(state, Bignum::create(state, 2)), Object::i2n(1));
    TS_ASSERT_EQUALS(Object::i2n(3)->compare(state, Bignum::create(state, 3)), Object::i2n(0));
    TS_ASSERT_EQUALS(Object::i2n(3)->compare(state, Bignum::create(state, 4)), Object::i2n(-1));
  }

  void test_compare_with_a_float() {
    TS_ASSERT_EQUALS(Object::i2n(3)->compare(state, Float::create(state, 2.9)), Object::i2n(1));
    TS_ASSERT_EQUALS(Object::i2n(3)->compare(state, Float::create(state, 3.0)), Object::i2n(0));
    TS_ASSERT_EQUALS(Object::i2n(3)->compare(state, Float::create(state, 3.1)), Object::i2n(-1));
  }

  void test_gt() {
    TS_ASSERT_EQUALS(Object::i2n(3)->gt(state, Object::i2n(2)), Qtrue);
    TS_ASSERT_EQUALS(Object::i2n(3)->gt(state, Object::i2n(3)), Qfalse);
  }

  void test_gt_with_a_bignum() {
    TS_ASSERT_EQUALS(Object::i2n(3)->gt(state, Bignum::create(state, 2)), Qtrue);
    TS_ASSERT_EQUALS(Object::i2n(3)->gt(state, Bignum::create(state, 3)), Qfalse);
  }

  void test_gt_with_a_float() {
    TS_ASSERT_EQUALS(Object::i2n(3)->gt(state, Float::create(state, 2.9)), Qtrue);
    TS_ASSERT_EQUALS(Object::i2n(3)->gt(state, Float::create(state, 3.0)), Qfalse);
  }

  void test_ge() {
    TS_ASSERT_EQUALS(Object::i2n(3)->ge(state, Object::i2n(2)), Qtrue);
    TS_ASSERT_EQUALS(Object::i2n(3)->ge(state, Object::i2n(3)), Qtrue);
    TS_ASSERT_EQUALS(Object::i2n(3)->ge(state, Object::i2n(4)), Qfalse);
  }

  void test_ge_with_a_bignum() {
    TS_ASSERT_EQUALS(Object::i2n(3)->ge(state, Bignum::create(state, 2)), Qtrue);
    TS_ASSERT_EQUALS(Object::i2n(3)->ge(state, Bignum::create(state, 3)), Qtrue);
    TS_ASSERT_EQUALS(Object::i2n(3)->ge(state, Bignum::create(state, 4)), Qfalse);
  }

  void test_ge_with_a_float() {
    TS_ASSERT_EQUALS(Object::i2n(3)->ge(state, Float::create(state, 2.9)), Qtrue);
    TS_ASSERT_EQUALS(Object::i2n(3)->ge(state, Float::create(state, 3.0)), Qtrue);
    TS_ASSERT_EQUALS(Object::i2n(3)->ge(state, Float::create(state, 3.1)), Qfalse);
  }

  void test_lt() {
    TS_ASSERT_EQUALS(Object::i2n(3)->lt(state, Object::i2n(4)), Qtrue);
    TS_ASSERT_EQUALS(Object::i2n(3)->lt(state, Object::i2n(3)), Qfalse);
  }

  void test_lt_with_a_bignum() {
    TS_ASSERT_EQUALS(Object::i2n(3)->lt(state, Bignum::create(state, 4)), Qtrue);
    TS_ASSERT_EQUALS(Object::i2n(3)->lt(state, Bignum::create(state, 3)), Qfalse);
  }

  void test_lt_with_a_float() {
    TS_ASSERT_EQUALS(Object::i2n(3)->lt(state, Float::create(state, 3.1)), Qtrue);
    TS_ASSERT_EQUALS(Object::i2n(3)->lt(state, Float::create(state, 3.0)), Qfalse);
  }

  void test_le() {
    TS_ASSERT_EQUALS(Object::i2n(3)->le(state, Object::i2n(4)), Qtrue);
    TS_ASSERT_EQUALS(Object::i2n(3)->le(state, Object::i2n(3)), Qtrue);
    TS_ASSERT_EQUALS(Object::i2n(3)->le(state, Object::i2n(2)), Qfalse);
  }

  void test_le_with_a_bignum() {
    TS_ASSERT_EQUALS(Object::i2n(3)->le(state, Bignum::create(state, 4)), Qtrue);
    TS_ASSERT_EQUALS(Object::i2n(3)->le(state, Bignum::create(state, 3)), Qtrue);
    TS_ASSERT_EQUALS(Object::i2n(3)->le(state, Bignum::create(state, 2)), Qfalse);
  }

  void test_le_with_a_float() {
    TS_ASSERT_EQUALS(Object::i2n(3)->le(state, Float::create(state, 3.1)), Qtrue);
    TS_ASSERT_EQUALS(Object::i2n(3)->le(state, Float::create(state, 3.0)), Qtrue);
    TS_ASSERT_EQUALS(Object::i2n(3)->le(state, Float::create(state, 2.9)), Qfalse);
  }

  void test_left_shift() {
    TS_ASSERT_EQUALS(Object::i2n(3)->left_shift(state, Object::i2n(4)), Object::i2n(48));

    INTEGER i = Object::i2n(1)->left_shift(state, Object::i2n(FIXNUM_WIDTH));
    TS_ASSERT_EQUALS(i->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(i->n2i(), FIXNUM_MAX + 1);

    TS_ASSERT_EQUALS(Object::i2n(34235)->left_shift(state, Object::i2n(-3)), Object::i2n(4279));
  }

  void test_right_shift() {
    TS_ASSERT_EQUALS(Object::i2n(49)->right_shift(state, Object::i2n(4)), Object::i2n(3));
    TS_ASSERT_EQUALS(Object::i2n(3)->right_shift(state, Object::i2n(-4)), Object::i2n(48));
    TS_ASSERT_EQUALS(Object::i2n(-49)->right_shift(state, Object::i2n(4)), Object::i2n(-4));
    TS_ASSERT_EQUALS(Object::i2n(1)->right_shift(state, Object::i2n(4)), Object::i2n(0));
    TS_ASSERT_EQUALS(Object::i2n(-1)->right_shift(state, Object::i2n(4)), Object::i2n(-1));
  }

  void test_size() {
    TS_ASSERT_EQUALS(static_cast<unsigned int>(Object::i2n(0)->size(state)->n2i()), 
		     sizeof(native_int));
  }

  void test_and() {
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_and(state, Object::i2n(3)), Object::i2n(1));
    TS_ASSERT_EQUALS(Object::i2n(-5)->bit_and(state, Object::i2n(3)), Object::i2n(3));
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_and(state, Object::i2n(-3)), Object::i2n(5));
    TS_ASSERT_EQUALS(Object::i2n(-5)->bit_and(state, Object::i2n(-3)), Object::i2n(-7));
  }

  void test_and_with_bignum() {
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_and(state, Bignum::create(state, 3)), Object::i2n(1));
    TS_ASSERT_EQUALS(Object::i2n(-5)->bit_and(state, Bignum::create(state, 3)), Object::i2n(3));
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_and(state, Bignum::create(state, -3)), Object::i2n(5));
    TS_ASSERT_EQUALS(Object::i2n(-5)->bit_and(state, Bignum::create(state, -3)), Object::i2n(-7));
  }

  void test_and_with_float() {
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_and(state, Float::create(state, 3.1)), Object::i2n(1));
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_and(state, Float::create(state, 2.9)), Object::i2n(0));
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_and(state, Float::create(state, -3.1)), Object::i2n(5));
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_and(state, Float::create(state, -2.9)), Object::i2n(4));
  }

  void test_or() {
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_or(state, Object::i2n(3)), Object::i2n(7));
    TS_ASSERT_EQUALS(Object::i2n(-5)->bit_or(state, Object::i2n(3)), Object::i2n(-5));
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_or(state, Object::i2n(-3)), Object::i2n(-3));
    TS_ASSERT_EQUALS(Object::i2n(-5)->bit_or(state, Object::i2n(-3)), Object::i2n(-1));
  }

  void test_or_with_bignum() {
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_or(state, Bignum::create(state, 3)), Object::i2n(7));
    TS_ASSERT_EQUALS(Object::i2n(-5)->bit_or(state, Bignum::create(state, 3)), Object::i2n(-5));
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_or(state, Bignum::create(state, -3)), Object::i2n(-3));
    TS_ASSERT_EQUALS(Object::i2n(-5)->bit_or(state, Bignum::create(state, -3)), Object::i2n(-1));
  }

  void test_or_with_float() {
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_or(state, Float::create(state, 3.1)), Object::i2n(7));
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_or(state, Float::create(state, 2.9)), Object::i2n(7));
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_or(state, Float::create(state, -3.1)), Object::i2n(-3));
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_or(state, Float::create(state, -2.9)), Object::i2n(-1));
  }

  void test_xor() {
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_xor(state, Object::i2n(3)), Object::i2n(6));
    TS_ASSERT_EQUALS(Object::i2n(-5)->bit_xor(state, Object::i2n(3)), Object::i2n(-8));
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_xor(state, Object::i2n(-3)), Object::i2n(-8));
    TS_ASSERT_EQUALS(Object::i2n(-5)->bit_xor(state, Object::i2n(-3)), Object::i2n(6));
  }

  void test_xor_with_bignum() {
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_xor(state, Bignum::create(state, 3)), Object::i2n(6));
    TS_ASSERT_EQUALS(Object::i2n(-5)->bit_xor(state, Bignum::create(state, 3)), Object::i2n(-8));
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_xor(state, Bignum::create(state, -3)), Object::i2n(-8));
    TS_ASSERT_EQUALS(Object::i2n(-5)->bit_xor(state, Bignum::create(state, -3)), Object::i2n(6));
  }

  void test_xor_with_float() {
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_xor(state, Float::create(state, 3.1)), Object::i2n(6));
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_xor(state, Float::create(state, 2.9)), Object::i2n(7));
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_xor(state, Float::create(state, -3.1)), Object::i2n(-8));
    TS_ASSERT_EQUALS(Object::i2n(5)->bit_xor(state, Float::create(state, -2.9)), Object::i2n(-5));
  }

  void test_invert() {
    TS_ASSERT_EQUALS(Object::i2n(5)->invert(state), Object::i2n(-6));
    TS_ASSERT_EQUALS(Object::i2n(-5)->invert(state), Object::i2n(4));
  }

  void test_to_f() {
    Float* f = Object::i2n(5)->to_f(state);
    check_float(f, Float::create(state, 5.0));
    f = Object::i2n(-2)->to_f(state);
    check_float(f, Float::create(state, -2.0));
  }

  void test_to_s() {
    String* n = Object::i2n(86545)->to_s(state);
    TS_ASSERT_EQUALS(std::string("86545"), (char*)*n);
  }

  void test_uncastable_object_throws_exception() {
    TS_ASSERT_THROWS( as<Integer>(String::create(state, "blah")), const TypeError &);
  }
};
