#include "builtin/list.hpp"
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

  void test_fixnum_fields() {
    TS_ASSERT_EQUALS(0, Fixnum::fields);
  }

  void check_float(Float* f, Float* g) {
    TS_ASSERT_RELATION(std::greater<double>, f->val + TOLERANCE, g->val);
    TS_ASSERT_RELATION(std::greater<double>, f->val, g->val - TOLERANCE);
    TS_ASSERT_RELATION(std::greater<double>, g->val + TOLERANCE, f->val);
    TS_ASSERT_RELATION(std::greater<double>, g->val, f->val - TOLERANCE);
  }

  void test_add() {
    FIXNUM one = as<Fixnum>(Fixnum::from(1));

    FIXNUM two = as<Fixnum>(one->add(state, one));
    TS_ASSERT_EQUALS(two->to_native(), 2);
  }
  
  void test_add_overflows_to_bignum() {
    FIXNUM  one = as<Fixnum>(Fixnum::from(FIXNUM_MAX - 10));
    INTEGER two = as<Integer>(one->add(state, one));

    TS_ASSERT_EQUALS(two->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(two->to_native(), (FIXNUM_MAX - 10) * 2);
  }

  void test_add_underflows_to_bignum() {
    FIXNUM  one = as<Fixnum>(Fixnum::from(FIXNUM_MIN + 10));
    INTEGER two = as<Integer>(one->add(state, one));

    TS_ASSERT_EQUALS(two->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(two->to_native(), (FIXNUM_MIN + 10) * 2);
  }

  void test_add_a_bignum() {
    FIXNUM one  = as<Fixnum>(Fixnum::from(13));
    Bignum* obj = Bignum::from(state, (native_int)FIXNUM_MAX - 10);
    INTEGER res = one->add(state, obj);
    TS_ASSERT_EQUALS(res->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(res->to_native(), FIXNUM_MAX + 3);
  }

  void test_add_a_float() {
    FIXNUM one = Fixnum::from(13);
    Float* res = one->add(state, Float::create(state, 1.4));
    check_float(res, Float::create(state, 14.4));
  }

  void test_sub() {
    FIXNUM one = as<Fixnum>(Fixnum::from(1));

    FIXNUM zero = as<Fixnum>(one->sub(state, one));
    TS_ASSERT_EQUALS(zero->to_native(), 0);
  }

  void test_sub_overflows_to_bignum() {
    FIXNUM  one   = as<Fixnum>(Fixnum::from(FIXNUM_MIN + 10));
    FIXNUM  two   = as<Fixnum>(Fixnum::from(FIXNUM_MAX - 10));
    INTEGER three = as<Integer>(two->sub(state, one));

    TS_ASSERT_EQUALS(three->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(three->to_native(), FIXNUM_MAX - 10 - (FIXNUM_MIN + 10));
  }

  void test_sub_underflows_to_bignum() {
    FIXNUM  one   = as<Fixnum>(Fixnum::from(FIXNUM_MIN + 10));
    FIXNUM  two   = as<Fixnum>(Fixnum::from(FIXNUM_MAX - 10));
    INTEGER three = as<Integer>(one->sub(state, two));

    TS_ASSERT_EQUALS(three->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(three->to_native(), FIXNUM_MIN + 10 - (FIXNUM_MAX - 10));
  }

  void test_sub_a_bignum() {
    FIXNUM one  = as<Fixnum>(Fixnum::from(13));
    Bignum* obj = Bignum::from(state, (native_int)FIXNUM_MAX + 28);
    INTEGER res = one->sub(state, obj);
    TS_ASSERT_EQUALS(res->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(res->to_native(), 13 - (FIXNUM_MAX + 28));
  }

  void test_sub_a_float() {
    FIXNUM one = Fixnum::from(13);
    Float* res = one->sub(state, Float::create(state, 1.4));
    check_float(res, Float::create(state, 11.6));
  }

  void test_mul() {
    FIXNUM one = as<Fixnum>(Fixnum::from(4));

    FIXNUM two = as<Fixnum>(one->mul(state, one));
    TS_ASSERT_EQUALS(two->to_native(), 16);
  }

  void test_mul_overflows_to_bignum() {
    FIXNUM  one = as<Fixnum>(Fixnum::from(FIXNUM_MAX - 10));
    INTEGER two = as<Integer>(one->mul(state, Fixnum::from(2)));

    TS_ASSERT_EQUALS(two->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(two->to_native(), (FIXNUM_MAX - 10) * 2);

    FIXNUM  three = as<Fixnum>(Fixnum::from(FIXNUM_MIN + 10));
    INTEGER four  = as<Integer>(three->mul(state, Fixnum::from(2)));

    TS_ASSERT_EQUALS(four->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(four->to_native(), (FIXNUM_MIN + 10) * 2);

    FIXNUM  five = as<Fixnum>(Fixnum::from(FIXNUM_MAX - 10));
    INTEGER six  = as<Integer>(five->mul(state, Fixnum::from(-2)));

    TS_ASSERT_EQUALS(six->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(six->to_native(), (FIXNUM_MAX - 10) * -2);

    FIXNUM  seven = as<Fixnum>(Fixnum::from(FIXNUM_MIN + 10));
    INTEGER eight = as<Integer>(seven->mul(state, Fixnum::from(-2)));

    TS_ASSERT_EQUALS(eight->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(eight->to_native(), (FIXNUM_MIN + 10) * -2);

  }

  void test_mul_with_bignum() {
    FIXNUM  one   = as<Fixnum>(Fixnum::from(2));
    Bignum* two   = Bignum::from(state, (native_int)FIXNUM_MAX + 10);
    INTEGER three = one->mul(state, two);
    
    TS_ASSERT_EQUALS(three->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(three->to_native(), (FIXNUM_MAX + 10) * 2);
  }

  void test_mul_a_float() {
    FIXNUM one = Fixnum::from(13);
    Float* res = one->mul(state, Float::create(state, 1.4));
    check_float(res, Float::create(state, 18.2));
  }

  void test_div() {
    FIXNUM one = as<Fixnum>(Fixnum::from(4));

    FIXNUM two = as<Fixnum>(one->div(state, one));
    TS_ASSERT_EQUALS(two->to_native(), 1);

    TS_ASSERT_THROWS( one->div(state, Fixnum::from(0)), const ZeroDivisionError &);
  }

  void test_div_a_bignum() {
    FIXNUM one = Fixnum::from(13);
    INTEGER res = one->div(state, Bignum::from(state, (native_int)FIXNUM_MAX + 10));
    TS_ASSERT_EQUALS(res->to_native(), 0);

    TS_ASSERT_THROWS( one->div(state, Bignum::from(state, (native_int)0)), const ZeroDivisionError &);
  }

  void test_div_a_float() {
    FIXNUM one = Fixnum::from(13);
    Float* res = one->div(state, Float::create(state, 1.4));
    check_float(res, Float::create(state, 9.28571428571));
  }

  void test_div_with_positive_arguments() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->div(state, Fixnum::from(2)), Fixnum::from(1));
  }

  void test_div_with_first_negative_argument() {
    TS_ASSERT_EQUALS(Fixnum::from(-3)->div(state, Fixnum::from(2)), Fixnum::from(-2));
  }

  void test_div_with_second_negative_argument() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->div(state, Fixnum::from(-2)), Fixnum::from(-2));
  }

  void test_div_with_negative_arguments() {
    TS_ASSERT_EQUALS(Fixnum::from(-3)->div(state, Fixnum::from(-2)), Fixnum::from(1));
  }

  void test_mod() {
    FIXNUM one = as<Fixnum>(Fixnum::from(4));

    FIXNUM two = as<Fixnum>(one->mod(state, one));
    TS_ASSERT_EQUALS(two->to_native(), 0);
  }

  void test_mod_with_positive_arguments() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->mod(state, Fixnum::from(2)), Fixnum::from(1));
  }

  void test_mod_with_first_negative_argument() {
    TS_ASSERT_EQUALS(Fixnum::from(-3)->mod(state, Fixnum::from(2)), Fixnum::from(1));
  }

  void test_mod_with_second_negative_argument() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->mod(state, Fixnum::from(-2)), Fixnum::from(-1));
  }

  void test_mod_with_negative_arguments() {
    TS_ASSERT_EQUALS(Fixnum::from(-3)->mod(state, Fixnum::from(-2)), Fixnum::from(-1));
  }

  void test_mod_with_a_bignum() {
    TS_ASSERT_EQUALS(Fixnum::from(-3)->mod(state, Bignum::from(state, (native_int)-2)), Fixnum::from(-1));
  }

  void test_mod_with_a_float() {
    Float* f = Fixnum::from(4)->mod(state, Float::create(state, -1.7));
    check_float(f, Float::create(state, -1.1));
  }

  void test_divmod() {
    FIXNUM one = as<Fixnum>(Fixnum::from(15));
    FIXNUM two = as<Fixnum>(Fixnum::from(4));

    Array* ary1 = one->divmod(state, two);
    Object* o1 = ary1->get(state, 0);
    Object* o2 = ary1->get(state, 1);
    TS_ASSERT(o1->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(o1)->to_native(), 3);
    TS_ASSERT(o2->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(o2)->to_native(), 3);

    TS_ASSERT_THROWS( one->divmod(state, Fixnum::from(0)), const ZeroDivisionError &);
  }

  void test_divmod_with_a_bignum() {
    FIXNUM one = as<Fixnum>(Fixnum::from(15));
    Bignum* two = Bignum::from(state, (native_int)4);

    Array* ary1 = one->divmod(state, two);
    Object* o1 = ary1->get(state, 0);
    Object* o2 = ary1->get(state, 1);
    TS_ASSERT(o1->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(o1)->to_native(), 3);
    TS_ASSERT(o2->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(o2)->to_native(), 3);
  }

  void test_divmod_with_a_float() {
    FIXNUM one = as<Fixnum>(Fixnum::from(15));

    Array* ary1 = one->divmod(state, Float::create(state, -3.3));
    Object* o1 = ary1->get(state, 0);
    Object* o2 = ary1->get(state, 1);
    TS_ASSERT(o1->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(o1)->to_native(), -5);
    check_float(as<Float>(o2), Float::create(state, -1.5));
  }

  void test_neg() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->neg(state),  Fixnum::from(-3));
    TS_ASSERT_EQUALS(Fixnum::from(-3)->neg(state),  Fixnum::from(3));
  }

  void test_equal() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->equal(state, Fixnum::from(3)), Qtrue);
    TS_ASSERT_EQUALS(Fixnum::from(4)->equal(state, Fixnum::from(3)), Qfalse);
  }

  void test_equal_with_a_bignum() {
    Bignum* obj1 = Bignum::from(state, (native_int)10);
    Bignum* obj2 = Bignum::from(state, (native_int)11);
    
    TS_ASSERT_EQUALS(Fixnum::from(10)->equal(state, obj1), Qtrue);
    TS_ASSERT_EQUALS(Fixnum::from(10)->equal(state, obj2), Qfalse);
  }

  void test_equal_with_a_float() {
    Float* obj1 = Float::create(state, 10.0);
    Float* obj2 = Float::create(state, 10.1);
    
    TS_ASSERT_EQUALS(Fixnum::from(10)->equal(state, obj1), Qtrue);
    TS_ASSERT_EQUALS(Fixnum::from(10)->equal(state, obj2), Qfalse);
  }

  void test_compare() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->compare(state, Fixnum::from(2)), Fixnum::from(1));
    TS_ASSERT_EQUALS(Fixnum::from(3)->compare(state, Fixnum::from(3)), Fixnum::from(0));
    TS_ASSERT_EQUALS(Fixnum::from(3)->compare(state, Fixnum::from(4)), Fixnum::from(-1));
  }

  void test_compare_with_a_bignum() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->compare(state, Bignum::from(state, (native_int)2)), Fixnum::from(1));
    TS_ASSERT_EQUALS(Fixnum::from(3)->compare(state, Bignum::from(state, (native_int)3)), Fixnum::from(0));
    TS_ASSERT_EQUALS(Fixnum::from(3)->compare(state, Bignum::from(state, (native_int)4)), Fixnum::from(-1));
  }

  void test_compare_with_a_float() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->compare(state, Float::create(state, 2.9)), Fixnum::from(1));
    TS_ASSERT_EQUALS(Fixnum::from(3)->compare(state, Float::create(state, 3.0)), Fixnum::from(0));
    TS_ASSERT_EQUALS(Fixnum::from(3)->compare(state, Float::create(state, 3.1)), Fixnum::from(-1));
  }

  void test_gt() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->gt(state, Fixnum::from(2)), Qtrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->gt(state, Fixnum::from(3)), Qfalse);
  }

  void test_gt_with_a_bignum() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->gt(state, Bignum::from(state, (native_int)2)), Qtrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->gt(state, Bignum::from(state, (native_int)3)), Qfalse);
  }

  void test_gt_with_a_float() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->gt(state, Float::create(state, 2.9)), Qtrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->gt(state, Float::create(state, 3.0)), Qfalse);
  }

  void test_ge() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->ge(state, Fixnum::from(2)), Qtrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->ge(state, Fixnum::from(3)), Qtrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->ge(state, Fixnum::from(4)), Qfalse);
  }

  void test_ge_with_a_bignum() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->ge(state, Bignum::from(state, (native_int)2)), Qtrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->ge(state, Bignum::from(state, (native_int)3)), Qtrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->ge(state, Bignum::from(state, (native_int)4)), Qfalse);
  }

  void test_ge_with_a_float() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->ge(state, Float::create(state, 2.9)), Qtrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->ge(state, Float::create(state, 3.0)), Qtrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->ge(state, Float::create(state, 3.1)), Qfalse);
  }

  void test_lt() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->lt(state, Fixnum::from(4)), Qtrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->lt(state, Fixnum::from(3)), Qfalse);
  }

  void test_lt_with_a_bignum() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->lt(state, Bignum::from(state, (native_int)4)), Qtrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->lt(state, Bignum::from(state, (native_int)3)), Qfalse);
  }

  void test_lt_with_a_float() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->lt(state, Float::create(state, 3.1)), Qtrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->lt(state, Float::create(state, 3.0)), Qfalse);
  }

  void test_le() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->le(state, Fixnum::from(4)), Qtrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->le(state, Fixnum::from(3)), Qtrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->le(state, Fixnum::from(2)), Qfalse);
  }

  void test_le_with_a_bignum() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->le(state, Bignum::from(state, (native_int)4)), Qtrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->le(state, Bignum::from(state, (native_int)3)), Qtrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->le(state, Bignum::from(state, (native_int)2)), Qfalse);
  }

  void test_le_with_a_float() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->le(state, Float::create(state, 3.1)), Qtrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->le(state, Float::create(state, 3.0)), Qtrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->le(state, Float::create(state, 2.9)), Qfalse);
  }

  void test_left_shift() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->left_shift(state, Fixnum::from(4)), Fixnum::from(48));

    INTEGER i = Fixnum::from(1)->left_shift(state, Fixnum::from(FIXNUM_WIDTH));
    TS_ASSERT_EQUALS(i->class_object(state), G(bignum));
    TS_ASSERT_EQUALS(i->to_native(), FIXNUM_MAX + 1);

    TS_ASSERT_EQUALS(Fixnum::from(34235)->left_shift(state, Fixnum::from(-3)), Fixnum::from(4279));
  }

  void test_right_shift() {
    TS_ASSERT_EQUALS(Fixnum::from(49)->right_shift(state, Fixnum::from(4)), Fixnum::from(3));
    TS_ASSERT_EQUALS(Fixnum::from(3)->right_shift(state, Fixnum::from(-4)), Fixnum::from(48));
    TS_ASSERT_EQUALS(Fixnum::from(-49)->right_shift(state, Fixnum::from(4)), Fixnum::from(-4));
    TS_ASSERT_EQUALS(Fixnum::from(1)->right_shift(state, Fixnum::from(4)), Fixnum::from(0));
    TS_ASSERT_EQUALS(Fixnum::from(-1)->right_shift(state, Fixnum::from(4)), Fixnum::from(-1));
  }

  void test_size() {
    TS_ASSERT_EQUALS(static_cast<unsigned int>(Fixnum::from(0)->size(state)->to_native()), 
		     sizeof(native_int));
  }

  void test_and() {
    TS_ASSERT_EQUALS(Fixnum::from( 5)->bit_and(state, Fixnum::from( 3)), Fixnum::from( 1));
    TS_ASSERT_EQUALS(Fixnum::from(-5)->bit_and(state, Fixnum::from( 3)), Fixnum::from( 3));
    TS_ASSERT_EQUALS(Fixnum::from( 5)->bit_and(state, Fixnum::from(-3)), Fixnum::from( 5));
    TS_ASSERT_EQUALS(Fixnum::from(-5)->bit_and(state, Fixnum::from(-3)), Fixnum::from(-7));
  }

  void test_and_with_bignum() {
    TS_ASSERT_EQUALS(Fixnum::from( 5)->bit_and(state, Bignum::from(state, (native_int) 3)), Fixnum::from( 1));
    TS_ASSERT_EQUALS(Fixnum::from(-5)->bit_and(state, Bignum::from(state, (native_int) 3)), Fixnum::from( 3));
    TS_ASSERT_EQUALS(Fixnum::from( 5)->bit_and(state, Bignum::from(state, (native_int)-3)), Fixnum::from( 5));
    TS_ASSERT_EQUALS(Fixnum::from(-5)->bit_and(state, Bignum::from(state, (native_int)-3)), Fixnum::from(-7));
  }

  void test_and_with_float() {
    TS_ASSERT_EQUALS(Fixnum::from(5)->bit_and(state, Float::create(state,  3.1)), Fixnum::from(1));
    TS_ASSERT_EQUALS(Fixnum::from(5)->bit_and(state, Float::create(state,  2.9)), Fixnum::from(0));
    TS_ASSERT_EQUALS(Fixnum::from(5)->bit_and(state, Float::create(state, -3.1)), Fixnum::from(5));
    TS_ASSERT_EQUALS(Fixnum::from(5)->bit_and(state, Float::create(state, -2.9)), Fixnum::from(4));
  }

  void test_or() {
    TS_ASSERT_EQUALS(Fixnum::from( 5)->bit_or(state, Fixnum::from( 3)), Fixnum::from( 7));
    TS_ASSERT_EQUALS(Fixnum::from(-5)->bit_or(state, Fixnum::from( 3)), Fixnum::from(-5));
    TS_ASSERT_EQUALS(Fixnum::from( 5)->bit_or(state, Fixnum::from(-3)), Fixnum::from(-3));
    TS_ASSERT_EQUALS(Fixnum::from(-5)->bit_or(state, Fixnum::from(-3)), Fixnum::from(-1));
  }

  void test_or_with_bignum() {
    TS_ASSERT_EQUALS(Fixnum::from( 5)->bit_or(state, Bignum::from(state, (native_int) 3)), Fixnum::from( 7));
    TS_ASSERT_EQUALS(Fixnum::from(-5)->bit_or(state, Bignum::from(state, (native_int) 3)), Fixnum::from(-5));
    TS_ASSERT_EQUALS(Fixnum::from( 5)->bit_or(state, Bignum::from(state, (native_int)-3)), Fixnum::from(-3));
    TS_ASSERT_EQUALS(Fixnum::from(-5)->bit_or(state, Bignum::from(state, (native_int)-3)), Fixnum::from(-1));
  }

  void test_or_with_float() {
    TS_ASSERT_EQUALS(Fixnum::from(5)->bit_or(state, Float::create(state,  3.1)), Fixnum::from( 7));
    TS_ASSERT_EQUALS(Fixnum::from(5)->bit_or(state, Float::create(state,  2.9)), Fixnum::from( 7));
    TS_ASSERT_EQUALS(Fixnum::from(5)->bit_or(state, Float::create(state, -3.1)), Fixnum::from(-3));
    TS_ASSERT_EQUALS(Fixnum::from(5)->bit_or(state, Float::create(state, -2.9)), Fixnum::from(-1));
  }

  void test_xor() {
    TS_ASSERT_EQUALS(Fixnum::from( 5)->bit_xor(state, Fixnum::from( 3)), Fixnum::from( 6));
    TS_ASSERT_EQUALS(Fixnum::from(-5)->bit_xor(state, Fixnum::from( 3)), Fixnum::from(-8));
    TS_ASSERT_EQUALS(Fixnum::from( 5)->bit_xor(state, Fixnum::from(-3)), Fixnum::from(-8));
    TS_ASSERT_EQUALS(Fixnum::from(-5)->bit_xor(state, Fixnum::from(-3)), Fixnum::from( 6));
  }

  void test_xor_with_bignum() {
    TS_ASSERT_EQUALS(Fixnum::from( 5)->bit_xor(state, Bignum::from(state, (native_int) 3)), Fixnum::from( 6));
    TS_ASSERT_EQUALS(Fixnum::from(-5)->bit_xor(state, Bignum::from(state, (native_int) 3)), Fixnum::from(-8));
    TS_ASSERT_EQUALS(Fixnum::from( 5)->bit_xor(state, Bignum::from(state, (native_int)-3)), Fixnum::from(-8));
    TS_ASSERT_EQUALS(Fixnum::from(-5)->bit_xor(state, Bignum::from(state, (native_int)-3)), Fixnum::from( 6));
  }

  void test_xor_with_float() {
    TS_ASSERT_EQUALS(Fixnum::from(5)->bit_xor(state, Float::create(state,  3.1)), Fixnum::from( 6));
    TS_ASSERT_EQUALS(Fixnum::from(5)->bit_xor(state, Float::create(state,  2.9)), Fixnum::from( 7));
    TS_ASSERT_EQUALS(Fixnum::from(5)->bit_xor(state, Float::create(state, -3.1)), Fixnum::from(-8));
    TS_ASSERT_EQUALS(Fixnum::from(5)->bit_xor(state, Float::create(state, -2.9)), Fixnum::from(-5));
  }

  void test_invert() {
    TS_ASSERT_EQUALS(Fixnum::from( 5)->invert(state), Fixnum::from(-6));
    TS_ASSERT_EQUALS(Fixnum::from(-5)->invert(state), Fixnum::from(4));
  }

  void test_to_f() {
    Float* f = Fixnum::from(5)->to_f(state);
    check_float(f, Float::create(state, 5.0));
    f = Fixnum::from(-2)->to_f(state);
    check_float(f, Float::create(state, -2.0));
  }

  void test_to_s() {
    String* n = Fixnum::from(86545)->to_s(state);
    TS_ASSERT_EQUALS(std::string("86545"), n->byte_address());
  }

  void test_coerce_fixnum() {
    FIXNUM one = Fixnum::from(1);
    FIXNUM two = Fixnum::from(2);
    Array* ary = one->coerce(state, two);

    FIXNUM a = try_as<Fixnum>(ary->get(state, 0));
    FIXNUM b = try_as<Fixnum>(ary->get(state, 1));

    TS_ASSERT_EQUALS(2U, ary->size());
    TS_ASSERT(a);
    TS_ASSERT(b);
    TS_ASSERT_EQUALS(two, a);
    TS_ASSERT_EQUALS(one, b);
  }

  void test_coerce_bignum() {
    FIXNUM one = Fixnum::from(1);
    Bignum* e = Bignum::create(state, one);
    Array* ary = one->coerce(state, e);

    FIXNUM a = try_as<Fixnum>(ary->get(state, 0));
    FIXNUM b = try_as<Fixnum>(ary->get(state, 1));

    TS_ASSERT_EQUALS(2U, ary->size());
    TS_ASSERT(a);
    TS_ASSERT(b);
    TS_ASSERT_EQUALS(one, a);
    TS_ASSERT_EQUALS(one, b);

    Bignum* f = Bignum::from(state, (native_int)2147483647);
    ary = one->coerce(state, f);
    Bignum* c = try_as<Bignum>(ary->get(state, 0));
    Bignum* d = try_as<Bignum>(ary->get(state, 1));

    TS_ASSERT_EQUALS(2U, ary->size());
    TS_ASSERT(c);
    TS_ASSERT(d);
    TS_ASSERT_EQUALS(Qtrue, c->equal(state, f));
    TS_ASSERT_EQUALS(Qtrue, d->equal(state, e));
  }

  void test_uncastable_object_throws_exception() {
    TS_ASSERT_THROWS( as<Integer>(String::create(state, "blah")), const TypeError &);
  }
};
