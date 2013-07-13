#include "vm/test/test.hpp"

#include "configuration.hpp"
#include "builtin/exception.hpp"
#include "builtin/list.hpp"
#include "version.h"

class TestFixnum : public CxxTest::TestSuite, public VMTest {
  public:

  double TOLERANCE;

  void setUp() {
    create();
    TOLERANCE = 0.00003;
  }

  void tearDown() {
    destroy();
  }

  void test_init() {
#ifdef IS_X8664
    size_t max_size =  4611686018427387903U;
    size_t min_size = -4611686018427387903U;
#else
    size_t max_size =  1073741823U;
    size_t min_size = -1073741823U;
#endif
    TS_ASSERT_EQUALS(Fixnum::from(max_size),
        G(fixnum_class)->get_const(state, "MAX"));
    TS_ASSERT_EQUALS(Fixnum::from(min_size),
        G(fixnum_class)->get_const(state, "MIN"));
  }

  void check_float(Float* f, Float* g) {
    TS_ASSERT_RELATION(std::greater<double>, f->val + TOLERANCE, g->val);
    TS_ASSERT_RELATION(std::greater<double>, f->val, g->val - TOLERANCE);
    TS_ASSERT_RELATION(std::greater<double>, g->val + TOLERANCE, f->val);
    TS_ASSERT_RELATION(std::greater<double>, g->val, f->val - TOLERANCE);
  }

  void test_add() {
    Fixnum* one = Fixnum::from(1);
    Fixnum* two = as<Fixnum>(one->add(state, one));

    TS_ASSERT_EQUALS(Fixnum::from(2), two);
    TS_ASSERT_EQUALS(2, two->to_native());
  }

  void test_add_overflows_to_bignum() {
    Fixnum* max = Fixnum::from(FIXNUM_MAX);
    Integer* max_plus1 = max->add(state, Fixnum::from(1));

    TS_ASSERT(kind_of<Bignum>(max_plus1));
    TS_ASSERT_EQUALS(FIXNUM_MAX+1, max_plus1->to_native());
  }

  void test_add_underflows_to_bignum() {
    Fixnum*  min = Fixnum::from(FIXNUM_MIN);
    Integer* min_minus1 = min->add(state, Fixnum::from(-1));

    TS_ASSERT(kind_of<Bignum>(min_minus1));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, min_minus1->to_native());
  }

  void test_add_a_bignum() {
    Fixnum* one  = Fixnum::from(13);
    Bignum* obj = Bignum::from(state, (native_int)FIXNUM_MAX - 10);
    Integer* res = one->add(state, obj);

    TS_ASSERT(kind_of<Bignum>(res));
    TS_ASSERT_EQUALS(res->to_native(), FIXNUM_MAX + 3);
  }

  void test_add_a_float() {
    Fixnum* one = Fixnum::from(13);
    Float* res = one->add(state, Float::create(state, 1.4));
    check_float(res, Float::create(state, 14.4));
  }

  void test_sub() {
    Fixnum* one = Fixnum::from(1);
    Fixnum* zero = as<Fixnum>(one->sub(state, one));

    TS_ASSERT_EQUALS(0, zero->to_native());
    TS_ASSERT_EQUALS(Fixnum::from(0), zero);
  }

  void test_sub_overflows_to_bignum() {
    Fixnum* max = Fixnum::from(FIXNUM_MAX);
    Integer* max_plus1 = max->sub(state, Fixnum::from(-1));

    TS_ASSERT(kind_of<Bignum>(max_plus1));
    TS_ASSERT_EQUALS(FIXNUM_MAX+1, max_plus1->to_native());
  }

  void test_sub_underflows_to_bignum() {
    Fixnum* min = Fixnum::from(FIXNUM_MIN);
    Integer* min_minus1 = min->sub(state, Fixnum::from(1));

    TS_ASSERT(kind_of<Bignum>(min_minus1));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, min_minus1->to_native());
  }

  void test_sub_a_bignum() {
    Fixnum* one  = as<Fixnum>(Fixnum::from(13));
    Bignum* obj = Bignum::from(state, (native_int)FIXNUM_MAX + 28);
    Integer* res = one->sub(state, obj);

    TS_ASSERT(kind_of<Bignum>(res));
    TS_ASSERT_EQUALS(res->to_native(), 13 - (FIXNUM_MAX + 28));
  }

  void test_sub_a_float() {
    Fixnum* one = Fixnum::from(13);
    Float* res = one->sub(state, Float::create(state, 1.4));
    check_float(res, Float::create(state, 11.6));
  }

  void test_mul() {
    Fixnum* third = Fixnum::from(FIXNUM_MAX / 3);
    Integer* max = third->mul(state, Fixnum::from(3));

    TS_ASSERT(kind_of<Fixnum>(max));
    TS_ASSERT_EQUALS(FIXNUM_MAX, max->to_native());

    Integer* min = third->mul(state, Fixnum::from(-3));

    TS_ASSERT(kind_of<Fixnum>(min));
    TS_ASSERT_EQUALS(FIXNUM_MIN, min->to_native());

    Fixnum* neg_third = Fixnum::from((FIXNUM_MIN) / 3);
    max = neg_third->mul(state, Fixnum::from(-3));

    TS_ASSERT(kind_of<Fixnum>(max));
    TS_ASSERT_EQUALS(FIXNUM_MAX, max->to_native());

    min = neg_third->mul(state, Fixnum::from(3));

    TS_ASSERT(kind_of<Fixnum>(min));
    TS_ASSERT_EQUALS(FIXNUM_MIN, min->to_native());
  }

  void test_mul_overflows_to_bignum() {
    Fixnum* half = Fixnum::from((FIXNUM_MAX + 1) / 2);
    Integer* max_plus1 = half->mul(state, Fixnum::from(2));

    TS_ASSERT(kind_of<Bignum>(max_plus1));
    TS_ASSERT_EQUALS(FIXNUM_MAX+1, max_plus1->to_native());

    Fixnum* neg_half = Fixnum::from((FIXNUM_MIN - 1) / 2);
    max_plus1 = neg_half->mul(state, Fixnum::from(-2));

    TS_ASSERT(kind_of<Bignum>(max_plus1));
    TS_ASSERT_EQUALS(FIXNUM_MAX+1, max_plus1->to_native());
  }

  void test_mul_underflows_to_bignum() {
    Fixnum* half = Fixnum::from((FIXNUM_MAX + 1) / 2);
    Integer* min_minus1 = half->mul(state, Fixnum::from(-2));

    TS_ASSERT(kind_of<Bignum>(min_minus1));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, min_minus1->to_native());

    Fixnum* neg_half = Fixnum::from((FIXNUM_MIN - 1) / 2);
    min_minus1 = neg_half->mul(state, Fixnum::from(2));

    TS_ASSERT(kind_of<Bignum>(min_minus1));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, min_minus1->to_native());
  }

  void test_mul_with_bignum() {
    Fixnum*  one   = as<Fixnum>(Fixnum::from(2));
    Bignum* two   = Bignum::from(state, (native_int)FIXNUM_MAX + 10);
    Integer* three = one->mul(state, two);

    TS_ASSERT_EQUALS(three->class_object(state), G(bignum));
    Bignum* expected = as<Bignum>(two->mul(state, Fixnum::from(2)));

    TS_ASSERT_EQUALS(cTrue, as<Bignum>(three)->equal(state, expected));
  }

  void test_mul_a_float() {
    Fixnum* one = Fixnum::from(13);
    Float* res = one->mul(state, Float::create(state, 1.4));
    check_float(res, Float::create(state, 18.2));
  }

  void test_div() {
    Fixnum* one = as<Fixnum>(Fixnum::from(4));

    Fixnum* two = as<Fixnum>(one->div(state, one));
    TS_ASSERT_EQUALS(two->to_native(), 1);

    Fixnum* zero = Fixnum::from(0);
    TS_ASSERT_THROWS_ASSERT(one->div(state, zero), const RubyException &e,
                            TS_ASSERT(Exception::zero_division_error_p(state, e.exception)));
  }

  void test_div_a_bignum() {
    Fixnum* one = Fixnum::from(13);
    Integer* res = one->div(state, Bignum::from(state, (native_int)FIXNUM_MAX + 10));
    TS_ASSERT_EQUALS(res->to_native(), 0);

    Bignum* zero = Bignum::from(state, (native_int)0);
    TS_ASSERT_THROWS_ASSERT(one->div(state, zero), const RubyException &e,
                            TS_ASSERT(Exception::zero_division_error_p(state, e.exception)));
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

  void test_div_close_to_zero() {
    TS_ASSERT_EQUALS(Fixnum::from(-1)->div(state, Fixnum::from(10)), Fixnum::from(-1));
  }

  void test_mod() {
    Fixnum* one = as<Fixnum>(Fixnum::from(4));

    Fixnum* two = as<Fixnum>(one->mod(state, one));
    TS_ASSERT_EQUALS(two->to_native(), 0);
  }

  void test_mod_with_positive_arguments() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->mod(state, Fixnum::from(2)), Fixnum::from(1));
    TS_ASSERT_EQUALS(Fixnum::from(200)->mod(state, Fixnum::from(256)), Fixnum::from(200));
  }

  void test_mod_with_first_negative_argument() {
    TS_ASSERT_EQUALS(Fixnum::from(-3)->mod(state, Fixnum::from(2)), Fixnum::from(1));
    TS_ASSERT_EQUALS(Fixnum::from(-200)->mod(state, Fixnum::from(256)), Fixnum::from(56));
  }

  void test_mod_with_second_negative_argument() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->mod(state, Fixnum::from(-2)), Fixnum::from(-1));
    TS_ASSERT_EQUALS(Fixnum::from(200)->mod(state, Fixnum::from(-256)), Fixnum::from(-56));
  }

  void test_mod_with_negative_arguments() {
    TS_ASSERT_EQUALS(Fixnum::from(-3)->mod(state, Fixnum::from(-2)), Fixnum::from(-1));
    TS_ASSERT_EQUALS(Fixnum::from(-200)->mod(state, Fixnum::from(-256)), Fixnum::from(-200));
  }

  void test_mod_with_a_bignum() {
    TS_ASSERT_EQUALS(Fixnum::from(-3)->mod(state, Bignum::from(state, (native_int)-2)), Fixnum::from(-1));
  }

  void test_mod_with_a_float() {
    Float* f = Fixnum::from(4)->mod(state, Float::create(state, -1.7));
    check_float(f, Float::create(state, -1.1));
  }

  void test_divmod() {
    Fixnum* one = as<Fixnum>(Fixnum::from(15));
    Fixnum* two = as<Fixnum>(Fixnum::from(4));

    Array* ary1 = one->divmod(state, two);
    Object* o1 = ary1->get(state, 0);
    Object* o2 = ary1->get(state, 1);
    TS_ASSERT(o1->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(o1)->to_native(), 3);
    TS_ASSERT(o2->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(o2)->to_native(), 3);

    Fixnum* zero = Fixnum::from(0);
    TS_ASSERT_THROWS_ASSERT(one->divmod(state, zero), const RubyException &e,
                            TS_ASSERT(Exception::zero_division_error_p(state, e.exception)));
  }

  void test_divmod_with_a_bignum() {
    Fixnum* one = as<Fixnum>(Fixnum::from(15));
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
    Fixnum* one = as<Fixnum>(Fixnum::from(15));

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

  void test_pow() {
    TS_ASSERT_EQUALS(Fixnum::from(0)->pow(state, Fixnum::from(1)), Fixnum::from(0));
    TS_ASSERT_EQUALS(Fixnum::from(7)->pow(state, Fixnum::from(0)), Fixnum::from(1));
    TS_ASSERT_EQUALS(Fixnum::from(-1)->pow(state, Fixnum::from(0)), Fixnum::from(1));
    TS_ASSERT_EQUALS(Fixnum::from(-1)->pow(state, Fixnum::from(1)), Fixnum::from(-1));
    TS_ASSERT_EQUALS(Fixnum::from(-1)->pow(state, Fixnum::from(2)), Fixnum::from(1));
    TS_ASSERT_EQUALS(Fixnum::from(7)->pow(state, Fixnum::from(5)), Fixnum::from(16807));
  }

  void test_pow_with_float() {
    if(LANGUAGE_18_ENABLED) {
      check_float(as<Float>(Fixnum::from(100)->pow(state, Fixnum::from(-1))),
          Float::create(state,.01));
    }
  }

  void test_pow_overflows_to_bignum() {
    Integer* fix = as<Integer>(Fixnum::from(2)->pow(state, Fixnum::from(FIXNUM_WIDTH-1)));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(Fixnum::from(1L << (FIXNUM_WIDTH-1)), fix);

    Integer* max_plus1 = as<Integer>(Fixnum::from(2)->pow(state, Fixnum::from(FIXNUM_WIDTH)));

    TS_ASSERT(kind_of<Bignum>(max_plus1));
    TS_ASSERT_EQUALS(FIXNUM_MAX + 1, max_plus1->to_native());
  }

  void test_pow_underflows_to_bignum() {
    Integer* fix = as<Integer>(Fixnum::from(-2)->pow(state, Fixnum::from(FIXNUM_WIDTH-1)));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(Fixnum::from(-1L << (FIXNUM_WIDTH-1)), fix);

    Integer* big = as<Integer>(Fixnum::from(-2)->pow(state, Fixnum::from(FIXNUM_WIDTH)));

    TS_ASSERT(kind_of<Bignum>(big));
    TS_ASSERT(Bignum::from(state, -1L << (FIXNUM_WIDTH))->equal(state, as<Bignum>(big)));
  }

  void test_equal() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->equal(state, Fixnum::from(3)), cTrue);
    TS_ASSERT_EQUALS(Fixnum::from(4)->equal(state, Fixnum::from(3)), cFalse);
  }

  void test_equal_with_a_bignum() {
    Bignum* obj1 = Bignum::from(state, (native_int)10);
    Bignum* obj2 = Bignum::from(state, (native_int)11);

    TS_ASSERT_EQUALS(Fixnum::from(10)->equal(state, obj1), cTrue);
    TS_ASSERT_EQUALS(Fixnum::from(10)->equal(state, obj2), cFalse);
  }

  void test_equal_with_a_float() {
    Float* obj1 = Float::create(state, 10.0);
    Float* obj2 = Float::create(state, 10.1);

    TS_ASSERT_EQUALS(Fixnum::from(10)->equal(state, obj1), cTrue);
    TS_ASSERT_EQUALS(Fixnum::from(10)->equal(state, obj2), cFalse);
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
    TS_ASSERT_EQUALS(Fixnum::from(3)->gt(state, Fixnum::from(2)), cTrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->gt(state, Fixnum::from(3)), cFalse);
  }

  void test_gt_with_a_bignum() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->gt(state, Bignum::from(state, (native_int)2)), cTrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->gt(state, Bignum::from(state, (native_int)3)), cFalse);
  }

  void test_gt_with_a_float() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->gt(state, Float::create(state, 2.9)), cTrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->gt(state, Float::create(state, 3.0)), cFalse);
  }

  void test_ge() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->ge(state, Fixnum::from(2)), cTrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->ge(state, Fixnum::from(3)), cTrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->ge(state, Fixnum::from(4)), cFalse);
  }

  void test_ge_with_a_bignum() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->ge(state, Bignum::from(state, (native_int)2)), cTrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->ge(state, Bignum::from(state, (native_int)3)), cTrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->ge(state, Bignum::from(state, (native_int)4)), cFalse);
  }

  void test_ge_with_a_float() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->ge(state, Float::create(state, 2.9)), cTrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->ge(state, Float::create(state, 3.0)), cTrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->ge(state, Float::create(state, 3.1)), cFalse);
  }

  void test_lt() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->lt(state, Fixnum::from(4)), cTrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->lt(state, Fixnum::from(3)), cFalse);
  }

  void test_lt_with_a_bignum() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->lt(state, Bignum::from(state, (native_int)4)), cTrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->lt(state, Bignum::from(state, (native_int)3)), cFalse);
  }

  void test_lt_with_a_float() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->lt(state, Float::create(state, 3.1)), cTrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->lt(state, Float::create(state, 3.0)), cFalse);
  }

  void test_le() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->le(state, Fixnum::from(4)), cTrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->le(state, Fixnum::from(3)), cTrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->le(state, Fixnum::from(2)), cFalse);
  }

  void test_le_with_a_bignum() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->le(state, Bignum::from(state, (native_int)4)), cTrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->le(state, Bignum::from(state, (native_int)3)), cTrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->le(state, Bignum::from(state, (native_int)2)), cFalse);
  }

  void test_le_with_a_float() {
    TS_ASSERT_EQUALS(Fixnum::from(3)->le(state, Float::create(state, 3.1)), cTrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->le(state, Float::create(state, 3.0)), cTrue);
    TS_ASSERT_EQUALS(Fixnum::from(3)->le(state, Float::create(state, 2.9)), cFalse);
  }

  void test_left_shift() {
    Fixnum* fix = as<Fixnum>(Fixnum::from(1)->left_shift(state, Fixnum::from(3)));
    TS_ASSERT_EQUALS(Fixnum::from(8), fix);

    fix = as<Fixnum>(Fixnum::from(8)->left_shift(state, Fixnum::from(-3)));
    TS_ASSERT_EQUALS(Fixnum::from(1), fix);

    fix = as<Fixnum>(Fixnum::from(-1)->left_shift(state, Fixnum::from(3)));
    TS_ASSERT_EQUALS(Fixnum::from(-8), fix);

    fix = as<Fixnum>(Fixnum::from(-8)->left_shift(state, Fixnum::from(-3)));
    TS_ASSERT_EQUALS(Fixnum::from(-1), fix);
  }

  void test_left_shift_overflows_to_bignum() {
    Integer* fix = Fixnum::from(1)->left_shift(state, Fixnum::from(FIXNUM_WIDTH-1));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(Fixnum::from(1L << (FIXNUM_WIDTH-1)), fix);

    fix = Fixnum::from(-1)->left_shift(state, Fixnum::from(FIXNUM_WIDTH-1));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(Fixnum::from(-1L << (FIXNUM_WIDTH-1)), fix);

    Integer* max_plus1 = Fixnum::from(1)->left_shift(state, Fixnum::from(FIXNUM_WIDTH));

    TS_ASSERT(kind_of<Bignum>(max_plus1));
    TS_ASSERT_EQUALS(FIXNUM_MAX + 1, max_plus1->to_native());

    Integer* min_minus1 = Fixnum::from(-1)->left_shift(state, Fixnum::from(FIXNUM_WIDTH));

    TS_ASSERT(kind_of<Bignum>(min_minus1));
    TS_ASSERT_EQUALS(FIXNUM_MIN - 1, min_minus1->to_native());
  }

  void test_right_shift() {
    Fixnum* fix = as<Fixnum>(Fixnum::from(8)->right_shift(state, Fixnum::from(3)));
    TS_ASSERT_EQUALS(Fixnum::from(1), fix);

    fix = as<Fixnum>(Fixnum::from(1)->right_shift(state, Fixnum::from(-3)));
    TS_ASSERT_EQUALS(Fixnum::from(8), fix);

    fix = as<Fixnum>(Fixnum::from(-8)->right_shift(state, Fixnum::from(3)));
    TS_ASSERT_EQUALS(Fixnum::from(-1), fix);

    fix = as<Fixnum>(Fixnum::from(-1)->right_shift(state, Fixnum::from(-3)));
    TS_ASSERT_EQUALS(Fixnum::from(-8), fix);
  }

  void test_right_shift_overflows_to_bignum() {
    Integer* fix = Fixnum::from(1)->right_shift(state, Fixnum::from(-(FIXNUM_WIDTH-1)));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(Fixnum::from(1L << (FIXNUM_WIDTH-1)), fix);

    fix = Fixnum::from(-1)->right_shift(state, Fixnum::from(-(FIXNUM_WIDTH-1)));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(Fixnum::from(-1L << (FIXNUM_WIDTH-1)), fix);

    Integer* max_plus1 = Fixnum::from(1)->right_shift(state, Fixnum::from(-FIXNUM_WIDTH));

    TS_ASSERT(kind_of<Bignum>(max_plus1));
    TS_ASSERT_EQUALS(FIXNUM_MAX + 1, max_plus1->to_native());

    Integer* min_minus1 = Fixnum::from(-1)->right_shift(state, Fixnum::from(-FIXNUM_WIDTH));

    TS_ASSERT(kind_of<Bignum>(min_minus1));
    TS_ASSERT_EQUALS(FIXNUM_MIN - 1, min_minus1->to_native());
  }

  void test_right_shift_moves_to_zero() {
    Integer* i = Fixnum::from(5)->right_shift(state,
                   Fixnum::from(sizeof(native_int) * 8));
    TS_ASSERT_EQUALS(i, Fixnum::from(0));
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

  void test_invert() {
    TS_ASSERT_EQUALS(Fixnum::from(FIXNUM_MAX)->invert(state), Fixnum::from(FIXNUM_MIN - 1));
    TS_ASSERT_EQUALS(Fixnum::from(FIXNUM_MIN)->invert(state), Fixnum::from(FIXNUM_MAX - 1));
  }

  void test_to_f() {
    Float* f = Fixnum::from(5)->to_f(state);
    check_float(f, Float::create(state, 5.0));
    f = Fixnum::from(-2)->to_f(state);
    check_float(f, Float::create(state, -2.0));
  }

  void test_to_s() {
    String* n = Fixnum::from(86545)->to_s(state);
    TS_ASSERT_EQUALS(std::string("86545"), n->c_str(state));
  }

  void test_coerce_fixnum() {
    Fixnum* one = Fixnum::from(1);
    Fixnum* two = Fixnum::from(2);
    Array* ary = one->coerce(state, two);

    Fixnum* a = try_as<Fixnum>(ary->get(state, 0));
    Fixnum* b = try_as<Fixnum>(ary->get(state, 1));

    TS_ASSERT_EQUALS(2, ary->size());
    TS_ASSERT(a);
    TS_ASSERT(b);
    TS_ASSERT_EQUALS(two, a);
    TS_ASSERT_EQUALS(one, b);
  }

  void test_coerce_bignum() {
    Fixnum* one = Fixnum::from(1);
    Bignum* e = Bignum::create(state, one);
    Array* ary = one->coerce(state, e);

    Fixnum* a = try_as<Fixnum>(ary->get(state, 0));
    Fixnum* b = try_as<Fixnum>(ary->get(state, 1));

    TS_ASSERT_EQUALS(2, ary->size());
    TS_ASSERT(a);
    TS_ASSERT(b);
    TS_ASSERT_EQUALS(one, a);
    TS_ASSERT_EQUALS(one, b);

    Bignum* f = Bignum::from(state, 9223372036854775807LL);
    ary = one->coerce(state, f);
    Bignum* c = try_as<Bignum>(ary->get(state, 0));
    Bignum* d = try_as<Bignum>(ary->get(state, 1));

    TS_ASSERT_EQUALS(2, ary->size());
    TS_ASSERT(c);
    TS_ASSERT(d);
    TS_ASSERT_EQUALS(cTrue, c->equal(state, f));
    TS_ASSERT_EQUALS(cTrue, d->equal(state, e));
  }

  void test_uncastable_object_throws_exception() {
    TS_ASSERT_THROWS( as<Integer>(String::create(state, "blah")), const TypeError &);
  }
};
