#include "vm/test/test.hpp"

#include "builtin/bignum.hpp"
#include "builtin/exception.hpp"
#include "builtin/float.hpp"
#include "builtin/string.hpp"

class TestBignum : public CxxTest::TestSuite, public VMTest {
  public:

  Bignum* b1;
  Bignum* b2;
  Fixnum* two;
  double TOLERANCE;

  void setUp() {
    create();
    b1 = Bignum::from(state, (native_int)2147483647);
    b2 = Bignum::from(state, (native_int)2147483646);
    two = Fixnum::from(2);
    TOLERANCE = 0.00003;
  }

  void tearDown() {
    destroy();
  }

  void test_from_int() {
    Bignum* obj = Bignum::from(state, 2147483647);
    TS_ASSERT_EQUALS(2147483647, obj->to_int());
    TS_ASSERT_EQUALS(2147483647, obj->to_native());
  }

  void test_from_int_negative() {
    // http://gcc.gnu.org/ml/gcc-bugs/2003-04/msg00082.html
    Bignum* obj = Bignum::from(state, 0x80000000);
    TS_ASSERT_EQUALS((int)0x80000000, obj->to_int());
  }

  void test_from_unsigned_int() {
    Bignum* obj = Bignum::from(state, 4294967295U);
    TS_ASSERT_EQUALS(4294967295U, obj->to_uint());
  }

  void test_from_long() {
    Bignum* obj = Bignum::from(state, 2147483647L);
    TS_ASSERT_EQUALS(2147483647L, obj->to_long());
  }

  void test_from_long_negative() {
    // http://gcc.gnu.org/ml/gcc-bugs/2003-04/msg00082.html
    Bignum* obj = Bignum::from(state, 0x80000000L);
    TS_ASSERT_EQUALS((long)0x80000000L, obj->to_long());
  }

  void test_from_unsigned_long() {
    Bignum* obj = Bignum::from(state, 4294967295LU);
    TS_ASSERT_EQUALS(4294967295LU, obj->to_ulong());
  }

  void test_from_long_long() {
    Bignum* obj = Bignum::from(state, 9223372036854775807LL);
    TS_ASSERT_EQUALS(9223372036854775807LL, obj->to_long_long());
  }

  void test_from_long_long_negative() {
    Bignum* obj = Bignum::from(state, 0x8000000000000000LL);
    TS_ASSERT_EQUALS((long long)0x8000000000000000LL, obj->to_long_long());
  }

  void test_from_unsigned_long_long() {
    Bignum* obj = Bignum::from(state, 18446744073709551615LLU);
    TS_ASSERT_EQUALS(18446744073709551615LLU, obj->to_ulong_long());
  }

  void test_from_mp_int() {
    Bignum* max = Bignum::from(state, FIXNUM_MAX);
    Integer* num = Bignum::from(state, max->mp_val());
    TS_ASSERT(kind_of<Fixnum>(num));

    Bignum* min = Bignum::from(state, FIXNUM_MIN);
    num = Bignum::from(state, min->mp_val());
    TS_ASSERT(kind_of<Fixnum>(num));

    Bignum* max_plus_one = Bignum::from(state, FIXNUM_MAX + 1);
    num = Bignum::from(state, max_plus_one->mp_val());
    TS_ASSERT(kind_of<Bignum>(num));

    Bignum* min_minus_one = Bignum::from(state, FIXNUM_MIN - 1);
    num = Bignum::from(state, min_minus_one->mp_val());
    TS_ASSERT(kind_of<Bignum>(num));
  }

  void test_create() {
    Bignum* obj = Bignum::create(state);
    TS_ASSERT_EQUALS(obj->class_object(state), G(bignum));
    TS_ASSERT(kind_of<Bignum>(obj));
  }

  void test_create_primitive() {
    Bignum* obj = Bignum::create(state, Fixnum::from(13));
    TS_ASSERT_EQUALS(obj->class_object(state), G(bignum));
    TS_ASSERT(kind_of<Bignum>(obj));
    TS_ASSERT_EQUALS(obj->to_native(), (native_int)13);
  }

  void test_dup() {
    Bignum* obj = Bignum::create(state, Fixnum::from(13));
    Bignum *dup = Bignum::create(state, Fixnum::from(0));
    dup->initialize_copy(state, obj);
    TS_ASSERT_EQUALS(obj->to_native(), dup->to_native());
  }

  void test_normalize() {
    Bignum* max_fix = Bignum::from(state, FIXNUM_MAX);
    Integer* max = Bignum::normalize(state, max_fix);

    TS_ASSERT(kind_of<Fixnum>(max));
    TS_ASSERT_EQUALS(Fixnum::from(FIXNUM_MAX), as<Fixnum>(max));

    Bignum* min_fix = Bignum::from(state, FIXNUM_MIN);
    Integer* min = Bignum::normalize(state, min_fix);

    TS_ASSERT(kind_of<Fixnum>(min));
    TS_ASSERT_EQUALS(Fixnum::from(FIXNUM_MIN), as<Fixnum>(min));

    Bignum* max_fix_plus1 = Bignum::from(state, FIXNUM_MAX+1);

    TS_ASSERT(kind_of<Bignum>(max_fix_plus1));
    TS_ASSERT(Bignum::from(state, FIXNUM_MAX+1)->equal(state, max_fix_plus1));

    Bignum* min_fix_minus1 = Bignum::from(state, FIXNUM_MIN-1);

    TS_ASSERT(kind_of<Bignum>(min_fix_minus1));
    TS_ASSERT(Bignum::from(state, FIXNUM_MIN-1)->equal(state, min_fix_minus1));
  }

  void check_float(Float* f, Float* g) {
    TS_ASSERT_DELTA(f->val, g->val, TOLERANCE);
  }

  void test_add_positive_range() {
    Bignum* fix = Bignum::from(state, FIXNUM_MAX-1);
    Integer* sum = fix->add(state, Bignum::from(state, 1));

    TS_ASSERT(kind_of<Fixnum>(sum));
    TS_ASSERT_EQUALS(FIXNUM_MAX, sum->to_native());

    fix = Bignum::from(state, FIXNUM_MAX+1);
    sum = fix->add(state, Bignum::from(state, -1));

    TS_ASSERT(kind_of<Fixnum>(sum));
    TS_ASSERT_EQUALS(FIXNUM_MAX, sum->to_native());

    fix = Bignum::from(state, FIXNUM_MAX);
    sum = fix->add(state, Bignum::from(state, 1));

    TS_ASSERT(kind_of<Bignum>(sum));
    TS_ASSERT_EQUALS(FIXNUM_MAX+1, sum->to_native());

    Bignum* big = Bignum::from(state, FIXNUM_MAX+2);
    sum = big->add(state, Bignum::from(state, -1));

    TS_ASSERT(kind_of<Bignum>(sum));
    TS_ASSERT_EQUALS(FIXNUM_MAX+1, sum->to_native());
  }

  void test_add_negative_range() {
    Bignum* fix = Bignum::from(state, FIXNUM_MIN-1);
    Integer* sum = fix->add(state, Bignum::from(state, 1));

    TS_ASSERT(kind_of<Fixnum>(sum));
    TS_ASSERT_EQUALS(FIXNUM_MIN, sum->to_native());

    fix = Bignum::from(state, FIXNUM_MIN+1);
    sum = fix->add(state, Bignum::from(state, -1));

    TS_ASSERT(kind_of<Fixnum>(sum));
    TS_ASSERT_EQUALS(FIXNUM_MIN, sum->to_native());

    fix = Bignum::from(state, FIXNUM_MIN-2);
    sum = fix->add(state, Bignum::from(state, 1));

    TS_ASSERT(kind_of<Bignum>(sum));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, sum->to_native());

    fix = Bignum::from(state, FIXNUM_MIN);
    sum = fix->add(state, Bignum::from(state, -1));

    TS_ASSERT(kind_of<Bignum>(sum));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, sum->to_native());
  }

  void test_add_with_positive_fixnum() {
    Fixnum* one = Fixnum::from(1);

    Bignum* fix = Bignum::from(state, FIXNUM_MAX-1);
    Integer* sum = fix->add(state, one);

    TS_ASSERT(kind_of<Fixnum>(sum));
    TS_ASSERT_EQUALS(FIXNUM_MAX, sum->to_native());

    fix = Bignum::from(state, FIXNUM_MIN-1);
    sum = fix->add(state, one);

    TS_ASSERT(kind_of<Fixnum>(sum));
    TS_ASSERT_EQUALS(FIXNUM_MIN, sum->to_native());

    Bignum* big = Bignum::from(state, FIXNUM_MAX);
    sum = big->add(state, one);

    TS_ASSERT(kind_of<Bignum>(sum));
    TS_ASSERT_EQUALS(FIXNUM_MAX+1, sum->to_native());

    big = Bignum::from(state, FIXNUM_MIN-2);
    sum = big->add(state, one);

    TS_ASSERT(kind_of<Bignum>(sum));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, sum->to_native());
  }

  void test_add_with_negative_fixnum() {
    Fixnum* neg_one = Fixnum::from(-1);

    Bignum* fix = Bignum::from(state, FIXNUM_MAX+1);
    Integer* sum = fix->add(state, neg_one);

    TS_ASSERT(kind_of<Fixnum>(sum));
    TS_ASSERT_EQUALS(FIXNUM_MAX, sum->to_native());

    fix = Bignum::from(state, FIXNUM_MIN+1);
    sum = fix->add(state, neg_one);

    TS_ASSERT(kind_of<Fixnum>(sum));
    TS_ASSERT_EQUALS(FIXNUM_MIN, sum->to_native());

    Bignum* big = Bignum::from(state, FIXNUM_MAX+2);
    sum = big->add(state, neg_one);

    TS_ASSERT(kind_of<Bignum>(sum));
    TS_ASSERT_EQUALS(FIXNUM_MAX+1, sum->to_native());

    big = Bignum::from(state, FIXNUM_MIN);
    sum = big->add(state, neg_one);

    TS_ASSERT(kind_of<Bignum>(sum));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, sum->to_native());
  }

  void test_add_with_float() {
    Float* f = Float::create(state, 0.2);
    Float* a = Bignum::from(state, (native_int)FIXNUM_MAX + 10)->add(state, f);
    check_float(a, Float::create(state, (double)FIXNUM_MAX + 10.2));
  }

  void test_sub_positive_range() {
    Bignum* fix = Bignum::from(state, FIXNUM_MAX+1);
    Integer* diff = fix->sub(state, Bignum::from(state, 1));

    TS_ASSERT(kind_of<Fixnum>(diff));
    TS_ASSERT_EQUALS(FIXNUM_MAX, diff->to_native());

    fix = Bignum::from(state, FIXNUM_MAX-1);
    diff = fix->sub(state, Bignum::from(state, -1));

    TS_ASSERT(kind_of<Fixnum>(diff));
    TS_ASSERT_EQUALS(FIXNUM_MAX, diff->to_native());

    fix = Bignum::from(state, FIXNUM_MAX+2);
    diff = fix->sub(state, Bignum::from(state, 1));

    TS_ASSERT(kind_of<Bignum>(diff));
    TS_ASSERT_EQUALS(FIXNUM_MAX+1, diff->to_native());

    Bignum* big = Bignum::from(state, FIXNUM_MAX);
    diff = big->sub(state, Bignum::from(state, -1));

    TS_ASSERT(kind_of<Bignum>(diff));
    TS_ASSERT_EQUALS(FIXNUM_MAX+1, diff->to_native());
  }

  void test_sub_negative_range() {
    Bignum* fix = Bignum::from(state, FIXNUM_MIN+1);
    Integer* diff = fix->sub(state, Bignum::from(state, 1));

    TS_ASSERT(kind_of<Fixnum>(diff));
    TS_ASSERT_EQUALS(FIXNUM_MIN, diff->to_native());

    fix = Bignum::from(state, FIXNUM_MIN-1);
    diff = fix->sub(state, Bignum::from(state, -1));

    TS_ASSERT(kind_of<Fixnum>(diff));
    TS_ASSERT_EQUALS(FIXNUM_MIN, diff->to_native());

    fix = Bignum::from(state, FIXNUM_MIN);
    diff = fix->sub(state, Bignum::from(state, 1));

    TS_ASSERT(kind_of<Bignum>(diff));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, diff->to_native());

    fix = Bignum::from(state, FIXNUM_MIN-2);
    diff = fix->sub(state, Bignum::from(state, -1));

    TS_ASSERT(kind_of<Bignum>(diff));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, diff->to_native());
  }

  void test_sub_with_positive_fixnum() {
    Fixnum* one = Fixnum::from(1);

    Bignum* fix = Bignum::from(state, FIXNUM_MAX+1);
    Integer* diff = fix->sub(state, one);

    TS_ASSERT(kind_of<Fixnum>(diff));
    TS_ASSERT_EQUALS(FIXNUM_MAX, diff->to_native());

    fix = Bignum::from(state, FIXNUM_MIN+1);
    diff = fix->sub(state, one);

    TS_ASSERT(kind_of<Fixnum>(diff));
    TS_ASSERT_EQUALS(FIXNUM_MIN, diff->to_native());

    Bignum* big = Bignum::from(state, FIXNUM_MAX+2);
    diff = big->sub(state, one);

    TS_ASSERT(kind_of<Bignum>(diff));
    TS_ASSERT_EQUALS(FIXNUM_MAX+1, diff->to_native());

    big = Bignum::from(state, FIXNUM_MIN);
    diff = big->sub(state, one);

    TS_ASSERT(kind_of<Bignum>(diff));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, diff->to_native());
  }

  void test_sub_with_negative_fixnum() {
    Fixnum* neg_one = Fixnum::from(-1);

    Bignum* fix = Bignum::from(state, FIXNUM_MAX-1);
    Integer* diff = fix->sub(state, neg_one);

    TS_ASSERT(kind_of<Fixnum>(diff));
    TS_ASSERT_EQUALS(FIXNUM_MAX, diff->to_native());

    fix = Bignum::from(state, FIXNUM_MIN-1);
    diff = fix->sub(state, neg_one);

    TS_ASSERT(kind_of<Fixnum>(diff));
    TS_ASSERT_EQUALS(FIXNUM_MIN, diff->to_native());

    Bignum* big = Bignum::from(state, FIXNUM_MAX);
    diff = big->sub(state, neg_one);

    TS_ASSERT(kind_of<Bignum>(diff));
    TS_ASSERT_EQUALS(FIXNUM_MAX+1, diff->to_native());

    big = Bignum::from(state, FIXNUM_MIN-2);
    diff = big->sub(state, neg_one);

    TS_ASSERT(kind_of<Bignum>(diff));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, diff->to_native());
  }

  void test_sub_with_float() {
    Float* f = Float::create(state, 0.2);
    Float* a = Bignum::from(state, (native_int)FIXNUM_MAX + 10)->sub(state, f);
    check_float(a, Float::create(state, (double)FIXNUM_MAX + 9.8));
  }

  void test_mul_positive_range() {
    Bignum* third = Bignum::from(state, FIXNUM_MAX / 3);
    Integer* max = third->mul(state, Bignum::from(state, 3));

    TS_ASSERT(kind_of<Fixnum>(max));
    TS_ASSERT_EQUALS(FIXNUM_MAX, max->to_native());

    Bignum* neg_third = Bignum::from(state, FIXNUM_MIN / 3);
    max = neg_third->mul(state, Bignum::from(state, -3));

    TS_ASSERT(kind_of<Fixnum>(max));
    TS_ASSERT_EQUALS(FIXNUM_MAX, max->to_native());

    Bignum* half = Bignum::from(state, (FIXNUM_MAX + 1) / 2);
    Integer* max_plus1 = half->mul(state, Bignum::from(state, 2));

    TS_ASSERT(kind_of<Bignum>(max_plus1));
    TS_ASSERT_EQUALS(FIXNUM_MAX+1, max_plus1->to_native());

    Bignum* neg_half = Bignum::from(state, (FIXNUM_MIN - 1) / 2);
    max_plus1 = neg_half->mul(state, Bignum::from(state, -2));

    TS_ASSERT(kind_of<Bignum>(max_plus1));
    TS_ASSERT_EQUALS(FIXNUM_MAX+1, max_plus1->to_native());
  }

  void test_mul_negative_range() {
    Bignum* third = Bignum::from(state, FIXNUM_MAX / 3);
    Integer* min = third->mul(state, Bignum::from(state, -3));

    TS_ASSERT(kind_of<Fixnum>(min));
    TS_ASSERT_EQUALS(FIXNUM_MIN, min->to_native());

    Bignum* neg_third = Bignum::from(state, FIXNUM_MIN / 3);
    min = neg_third->mul(state, Bignum::from(state, 3));

    TS_ASSERT(kind_of<Fixnum>(min));
    TS_ASSERT_EQUALS(FIXNUM_MIN, min->to_native());

    Bignum* half = Bignum::from(state, (FIXNUM_MAX + 1) / 2);
    Integer* min_minus1 = half->mul(state, Bignum::from(state, -2));

    TS_ASSERT(kind_of<Bignum>(min_minus1));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, min_minus1->to_native());

    Bignum* neg_half = Bignum::from(state, (FIXNUM_MIN - 1) / 2);
    min_minus1 = neg_half->mul(state, Bignum::from(state, 2));

    TS_ASSERT(kind_of<Bignum>(min_minus1));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, min_minus1->to_native());
  }

  void test_mul_with_positive_fixnum() {
    Fixnum* two = Fixnum::from(2);
    Fixnum* three = Fixnum::from(3);

    Bignum* half = Bignum::from(state, (FIXNUM_MAX + 1) / 2);
    Integer* max_plus1 = half->mul(state, two);

    TS_ASSERT(kind_of<Bignum>(max_plus1));
    TS_ASSERT_EQUALS(FIXNUM_MAX+1, max_plus1->to_native());

    Bignum* neg_half = Bignum::from(state, (FIXNUM_MIN - 1) / 2);
    Integer* min_minus1 = neg_half->mul(state, two);

    TS_ASSERT(kind_of<Bignum>(min_minus1));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, min_minus1->to_native());

    Bignum* third = Bignum::from(state, FIXNUM_MAX / 3);
    Integer* max = third->mul(state, three);

    TS_ASSERT(kind_of<Fixnum>(max));
    TS_ASSERT_EQUALS(FIXNUM_MAX, max->to_native());

    Bignum* neg_third = Bignum::from(state, FIXNUM_MIN / 3);
    Integer* min = neg_third->mul(state, three);

    TS_ASSERT(kind_of<Fixnum>(min));
    TS_ASSERT_EQUALS(FIXNUM_MIN, min->to_native());
  }

  void test_mul_with_negative_fixnum() {
    Fixnum* neg_two = Fixnum::from(-2);
    Fixnum* neg_three = Fixnum::from(-3);

    Bignum* neg_half = Bignum::from(state, (FIXNUM_MIN - 1) / 2);
    Integer* max_plus1 = neg_half->mul(state, neg_two);

    TS_ASSERT(kind_of<Bignum>(max_plus1));
    TS_ASSERT_EQUALS(FIXNUM_MAX+1, max_plus1->to_native());

    Bignum* half = Bignum::from(state, (FIXNUM_MAX + 1) / 2);
    Integer* min_minus1 = half->mul(state, neg_two);

    TS_ASSERT(kind_of<Bignum>(min_minus1));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, min_minus1->to_native());

    Bignum* neg_third = Bignum::from(state, FIXNUM_MIN / 3);
    Integer* max = neg_third->mul(state, neg_three);

    TS_ASSERT(kind_of<Fixnum>(max));
    TS_ASSERT_EQUALS(FIXNUM_MAX, max->to_native());

    Bignum* third = Bignum::from(state, FIXNUM_MAX / 3);
    Integer* min = third->mul(state, neg_three);

    TS_ASSERT(kind_of<Fixnum>(min));
    TS_ASSERT_EQUALS(FIXNUM_MIN, min->to_native());
  }

  void test_mul_with_float() {
    Float* f = Float::create(state, 0.2);
    Float* a = Bignum::from(state, (native_int)FIXNUM_MAX + 10)->mul(state, f);
    check_float(a, Float::create(state, ((double)FIXNUM_MAX + 10) * 0.2));
  }

  void test_div() {
    Bignum* two = Bignum::from(state, 2);
    Bignum* neg_two = Bignum::from(state, -2);

    unsigned long big_int = FIXNUM_MAX + 1;

    Bignum* big = Bignum::from(state, big_int * 2);
    Integer* max_plus1 = big->div(state, two);

    TS_ASSERT(kind_of<Bignum>(max_plus1));
    TS_ASSERT_EQUALS(FIXNUM_MAX+1, max_plus1->to_native());

    Integer* min_minus1 = big->div(state, neg_two);

    TS_ASSERT(kind_of<Bignum>(min_minus1));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, min_minus1->to_native());

    big = Bignum::from(state, FIXNUM_MAX * 2);
    Integer* max = big->div(state, two);

    TS_ASSERT(kind_of<Fixnum>(max));
    TS_ASSERT_EQUALS(FIXNUM_MAX, max->to_native());

    Integer* min = big->div(state, neg_two);

    TS_ASSERT(kind_of<Fixnum>(min));
    TS_ASSERT_EQUALS(FIXNUM_MIN, min->to_native());

    Bignum* zero = Bignum::from(state, (native_int)0);
    TS_ASSERT_THROWS_ASSERT(b1->div(state, zero), const RubyException &e,
        TS_ASSERT(Exception::zero_division_error_p(state, e.exception)));
  }

  void test_div_with_fixnum() {
    Fixnum* two = Fixnum::from(2);
    Fixnum* neg_two = Fixnum::from(-2);

    unsigned long long big_int = FIXNUM_MAX + 1;

    Bignum* big = Bignum::from(state, big_int * 2);
    Integer* max_plus1 = big->div(state, two);

    TS_ASSERT(kind_of<Bignum>(max_plus1));
    TS_ASSERT_EQUALS(FIXNUM_MAX+1, max_plus1->to_native());

    Integer* min_minus1 = big->div(state, neg_two);

    TS_ASSERT(kind_of<Bignum>(min_minus1));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, min_minus1->to_native());

    big = Bignum::from(state, FIXNUM_MAX * 2);
    Integer* max = big->div(state, two);

    TS_ASSERT(kind_of<Fixnum>(max));
    TS_ASSERT_EQUALS(FIXNUM_MAX, max->to_native());

    Integer* min = big->div(state, neg_two);

    TS_ASSERT(kind_of<Fixnum>(min));
    TS_ASSERT_EQUALS(FIXNUM_MIN, min->to_native());

    Fixnum* zero = Fixnum::from(0);
    TS_ASSERT_THROWS_ASSERT(b1->div(state, zero), const RubyException &e,
        TS_ASSERT(Exception::zero_division_error_p(state, e.exception)));
  }

  void test_divmod() {
    Array* ary = b1->divmod(state, b1);
    TS_ASSERT_EQUALS(1, as<Fixnum>(ary->get(state, 0))->to_native());
    TS_ASSERT_EQUALS(0, as<Fixnum>(ary->get(state, 1))->to_native());

    ary = b1->divmod(state, two);
    TS_ASSERT_EQUALS(1073741823, as<Fixnum>(ary->get(state, 0))->to_native());
    TS_ASSERT_EQUALS(1, as<Fixnum>(ary->get(state, 1))->to_native());

    Bignum* nbn1 = Bignum::from(state, -2147483647);
    ary = b1->divmod(state, nbn1);
    TS_ASSERT_EQUALS(-1, as<Fixnum>(ary->get(state, 0))->to_native());
    TS_ASSERT_EQUALS(0, as<Fixnum>(ary->get(state, 1))->to_native());

    ary = nbn1->divmod(state, b1);
    TS_ASSERT_EQUALS(-1, as<Fixnum>(ary->get(state, 0))->to_native());
    TS_ASSERT_EQUALS(0, as<Fixnum>(ary->get(state, 1))->to_native());

    ary = nbn1->divmod(state, nbn1);
    TS_ASSERT_EQUALS(1, as<Fixnum>(ary->get(state, 0))->to_native());
    TS_ASSERT_EQUALS(0, as<Fixnum>(ary->get(state, 1))->to_native());

    ary = nbn1->divmod(state, Bignum::from(state, 89478485));
    TS_ASSERT_EQUALS(-25, as<Fixnum>(ary->get(state, 0))->to_native());
    TS_ASSERT_EQUALS(89478478, as<Fixnum>(ary->get(state, 1))->to_native());

    ary = nbn1->divmod(state, Fixnum::from(89478485));
    TS_ASSERT_EQUALS(-25, as<Fixnum>(ary->get(state, 0))->to_native());
    TS_ASSERT_EQUALS(89478478, as<Fixnum>(ary->get(state, 1))->to_native());

    ary = b1->divmod(state, Fixnum::from(-89478485));
    TS_ASSERT_EQUALS(-25, as<Fixnum>(ary->get(state, 0))->to_native());
    TS_ASSERT_EQUALS(-89478478, as<Fixnum>(ary->get(state, 1))->to_native());

    ary = b1->divmod(state, Float::create(state, 2.5));
    TS_ASSERT_EQUALS(858993458, as<Fixnum>(ary->get(state, 0))->to_native());
    check_float(as<Float>(ary->get(state, 1)), Float::create(state, 2.0));
  }

  void test_mod() {
    Bignum* neg = Bignum::from(state, FIXNUM_MIN-2);
    Integer* fix = neg->mod(state, Bignum::from(state, FIXNUM_MAX+1));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(FIXNUM_MAX, fix->to_native());

    Bignum* pos = Bignum::from(state, FIXNUM_MAX+2);
    fix = pos->mod(state, Bignum::from(state, FIXNUM_MIN-1));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(FIXNUM_MIN, fix->to_native());

    neg = Bignum::from(state, FIXNUM_MIN-3);
    Integer* big = neg->mod(state, Bignum::from(state, FIXNUM_MAX+2));

    TS_ASSERT(kind_of<Bignum>(big));
    TS_ASSERT_EQUALS(FIXNUM_MAX+1, big->to_native());

    pos = Bignum::from(state, FIXNUM_MAX+3);
    big = pos->mod(state, Bignum::from(state, FIXNUM_MIN-2));

    TS_ASSERT(kind_of<Bignum>(big));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, big->to_native());
  }

  void test_mod_with_fixnum() {
    Bignum* neg = Bignum::from(state, FIXNUM_MIN-1);
    Integer* fix = neg->mod(state, Fixnum::from(FIXNUM_MAX));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(FIXNUM_MAX-1, fix->to_native());

    Bignum* pos = Bignum::from(state, FIXNUM_MAX+1);
    fix = pos->mod(state, Fixnum::from(FIXNUM_MIN));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(FIXNUM_MIN+1, fix->to_native());
  }

  void test_mod_with_float() {
    Float* f = b1->mod(state, Float::create(state, -21483645.2));
    check_float(f, Float::create(state, -880872.999999925));
  }

  void test_left_shift() {
    Bignum* one = Bignum::from(state, 1);
    Bignum* neg_one = Bignum::from(state, -1);

    Fixnum* width_minus1 = Fixnum::from(FIXNUM_WIDTH-1);
    Fixnum* width = Fixnum::from(FIXNUM_WIDTH);

    Integer* fix = one->left_shift(state, width_minus1);

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(1L << (FIXNUM_WIDTH-1), fix->to_native());

    fix = neg_one->left_shift(state, width_minus1);

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(-1L << (FIXNUM_WIDTH-1), fix->to_native());

    Integer* max_plus1 = one->left_shift(state, width);

    TS_ASSERT(kind_of<Bignum>(max_plus1));
    TS_ASSERT_EQUALS(FIXNUM_MAX + 1, max_plus1->to_native());

    Integer* min_minus1 = neg_one->left_shift(state, width);

    TS_ASSERT(kind_of<Bignum>(min_minus1));
    TS_ASSERT_EQUALS(FIXNUM_MIN - 1, min_minus1->to_native());
  }

  void test_right_shift() {
    Bignum* one = Bignum::from(state, 1);
    Bignum* neg_one = Bignum::from(state, -1);

    Fixnum* neg_width_minus1 = Fixnum::from(-(FIXNUM_WIDTH-1));
    Fixnum* neg_width = Fixnum::from(-FIXNUM_WIDTH);

    Integer* fix = one->right_shift(state, neg_width_minus1);

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(1L << (FIXNUM_WIDTH-1), fix->to_native());

    fix = neg_one->right_shift(state, neg_width_minus1);

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(-1L << (FIXNUM_WIDTH-1), fix->to_native());

    Integer* max_plus1 = one->right_shift(state, neg_width);

    TS_ASSERT(kind_of<Bignum>(max_plus1));
    TS_ASSERT_EQUALS(FIXNUM_MAX + 1, max_plus1->to_native());

    Integer* min_minus1 = neg_one->right_shift(state, neg_width);

    TS_ASSERT(kind_of<Bignum>(min_minus1));
    TS_ASSERT_EQUALS(FIXNUM_MIN - 1, min_minus1->to_native());
  }

  void test_pow() {
    Bignum* two = Bignum::from(state, 2);
    Bignum* neg_two = Bignum::from(state, -2);

    Fixnum* width_minus1 = Fixnum::from(FIXNUM_WIDTH-1);
    Fixnum* width = Fixnum::from(FIXNUM_WIDTH);

    Integer* fix = as<Integer>(two->pow(state, width_minus1));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(1L << (FIXNUM_WIDTH-1), fix->to_native());

    fix = as<Integer>(neg_two->pow(state, width_minus1));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(-1L << (FIXNUM_WIDTH-1), fix->to_native());

    Integer* max_plus1 = as<Integer>(two->pow(state, width));

    TS_ASSERT(kind_of<Bignum>(max_plus1));
    TS_ASSERT_EQUALS(FIXNUM_MAX + 1, max_plus1->to_native());

    Integer* big = as<Integer>(neg_two->pow(state, width));

    TS_ASSERT(kind_of<Bignum>(big));
    TS_ASSERT_EQUALS(1L << (FIXNUM_WIDTH), big->to_native());

    big = as<Integer>(neg_two->pow(state, Fixnum::from(FIXNUM_WIDTH+1)));

    TS_ASSERT(kind_of<Bignum>(big));
    TS_ASSERT_EQUALS(-1LL << (FIXNUM_WIDTH+1), as<Bignum>(big)->to_long_long());
  }

  void test_equal() {
    TS_ASSERT_EQUALS(b1->equal(state, b1), cTrue);
    TS_ASSERT_EQUALS(b1->equal(state, b2), cFalse);

    Bignum* b = Bignum::from(state, (native_int)2);
    TS_ASSERT_EQUALS(b->equal(state, Fixnum::from(2)), cTrue);
    TS_ASSERT_EQUALS(b->equal(state, Fixnum::from(1)), cFalse);

    b = Bignum::from(state, (native_int)-2);
    TS_ASSERT_EQUALS(b->equal(state, Fixnum::from(-2)), cTrue);
    TS_ASSERT_EQUALS(b->equal(state, Fixnum::from(1)), cFalse);

    TS_ASSERT_EQUALS(b->equal(state, Float::create(state, -2.0)), cTrue);
    TS_ASSERT_EQUALS(b->equal(state, Float::create(state, 1.9)), cFalse);
  }

  void test_compare() {
    TS_ASSERT_EQUALS(b1->compare(state, b2), Fixnum::from(1));
    TS_ASSERT_EQUALS(b2->compare(state, b1), Fixnum::from(-1));
    TS_ASSERT_EQUALS(b1->compare(state, b1), Fixnum::from(0));

    TS_ASSERT_EQUALS(b1->compare(state, Fixnum::from(0)), Fixnum::from(1));
  }

  void test_compare_with_fixnum() {
    TS_ASSERT_EQUALS(Fixnum::from(0),
        Bignum::from(state, (native_int)3)->compare(state, Fixnum::from(3)));
    TS_ASSERT_EQUALS(Fixnum::from(1),
        Bignum::from(state, (native_int)3)->compare(state, Fixnum::from(2)));
    TS_ASSERT_EQUALS(Fixnum::from(-1),
        Bignum::from(state, (native_int)3)->compare(state, Fixnum::from(4)));

    TS_ASSERT_EQUALS(Fixnum::from(0),
        Bignum::from(state, (native_int)-3)->compare(state, Fixnum::from(-3)));
    TS_ASSERT_EQUALS(Fixnum::from(-1),
        Bignum::from(state, (native_int)-3)->compare(state, Fixnum::from(-2)));
    TS_ASSERT_EQUALS(Fixnum::from(1),
        Bignum::from(state, (native_int)-3)->compare(state, Fixnum::from(-4)));
  }

  void test_compare_with_float() {
    TS_ASSERT_EQUALS(Fixnum::from(0),
        Bignum::from(state, (native_int)3)->compare(state, Float::create(state, 3.0)));
    TS_ASSERT_EQUALS(Fixnum::from(1),
        Bignum::from(state, (native_int)3)->compare(state, Float::create(state, 2.9)));
    TS_ASSERT_EQUALS(Fixnum::from(-1),
        Bignum::from(state, (native_int)3)->compare(state, Float::create(state, 3.1)));

    TS_ASSERT_EQUALS(Fixnum::from(0),
        Bignum::from(state, (native_int)-3)->compare(state, Float::create(state, -3.0)));
    TS_ASSERT_EQUALS(Fixnum::from(-1),
        Bignum::from(state, (native_int)-3)->compare(state, Float::create(state, -2.9)));
    TS_ASSERT_EQUALS(Fixnum::from(1),
        Bignum::from(state, (native_int)-3)->compare(state, Float::create(state, -3.1)));
  }

  void test_neg() {
    Bignum* big = Bignum::from(state, FIXNUM_MAX+1);
    Integer* negated = big->neg(state);

    TS_ASSERT(kind_of<Bignum>(negated));
    TS_ASSERT_EQUALS(-(FIXNUM_MAX+1), negated->to_native());

    Bignum* fix = Bignum::from(state, FIXNUM_MAX);
    negated = fix->neg(state);

    TS_ASSERT(kind_of<Fixnum>(negated));
    TS_ASSERT_EQUALS(-FIXNUM_MAX, negated->to_native());
  }

  void test_invert() {
    Bignum* fix = Bignum::from(state, FIXNUM_MAX-1);
    Integer* inverted = fix->invert(state);

    TS_ASSERT(kind_of<Fixnum>(inverted));
    TS_ASSERT_EQUALS(FIXNUM_MIN, inverted->to_native());

    fix = Bignum::from(state, FIXNUM_MAX);
    inverted = fix->invert(state);

    TS_ASSERT(kind_of<Bignum>(inverted));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, inverted->to_native());
  }

  void test_bit_and() {
    Bignum* pos = Bignum::from(state, (1L<<FIXNUM_WIDTH)|(1L<<(FIXNUM_WIDTH-1)));
    Integer* fix = pos->bit_and(state, Bignum::from(state, FIXNUM_MAX));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(1L<<(FIXNUM_WIDTH-1), fix->to_native());

    Bignum* neg = Bignum::from(state, FIXNUM_MIN);
    fix = neg->bit_and(state, Bignum::from(state, FIXNUM_MIN));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(FIXNUM_MIN, fix->to_native());

    Integer* big = pos->bit_and(state, Bignum::from(state, (FIXNUM_MAX+1)));

    TS_ASSERT(kind_of<Bignum>(big));
    TS_ASSERT_EQUALS(1L<<FIXNUM_WIDTH, big->to_native());

    neg = Bignum::from(state, -1L << FIXNUM_WIDTH);
    big = neg->bit_and(state, Bignum::from(state, FIXNUM_MIN-1));

    TS_ASSERT(kind_of<Bignum>(big));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, big->to_native());
  }

  void test_bit_and_with_fixnum() {
    Bignum* pos = Bignum::from(state, (1L<<FIXNUM_WIDTH)|(1L<<(FIXNUM_WIDTH-1)));
    Integer* fix = pos->bit_and(state, Fixnum::from(FIXNUM_MAX));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(1L<<(FIXNUM_WIDTH-1), fix->to_native());

    Bignum* neg = Bignum::from(state, FIXNUM_MIN);
    fix = neg->bit_and(state, Fixnum::from(FIXNUM_MIN));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(FIXNUM_MIN, fix->to_native());
  }

  void test_bit_or() {
    Bignum* pos = Bignum::from(state, 1);
    Integer* fix = pos->bit_or(state, Bignum::from(state, FIXNUM_MAX));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(FIXNUM_MAX, fix->to_native());

    Bignum* neg = Bignum::from(state, FIXNUM_MIN);
    fix = neg->bit_or(state, Bignum::from(state, FIXNUM_MIN));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(FIXNUM_MIN, fix->to_native());

    Integer* big = pos->bit_or(state, Bignum::from(state, (FIXNUM_MAX+1)));

    TS_ASSERT(kind_of<Bignum>(big));
    TS_ASSERT_EQUALS((1L<<FIXNUM_WIDTH)|1, big->to_native());

    neg = Bignum::from(state, -1L << FIXNUM_WIDTH);
    big = neg->bit_or(state, Bignum::from(state, FIXNUM_MIN-1));

    TS_ASSERT(kind_of<Bignum>(big));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, big->to_native());
  }

  void test_bit_or_with_fixnum() {
    Bignum* pos = Bignum::from(state, 1);
    Integer* fix = pos->bit_or(state, Fixnum::from(FIXNUM_MAX));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(FIXNUM_MAX, fix->to_native());

    Bignum* neg = Bignum::from(state, FIXNUM_MIN);
    fix = neg->bit_or(state, Fixnum::from(FIXNUM_MIN));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(FIXNUM_MIN, fix->to_native());
  }

  void test_bit_xor() {
    Bignum* pos = Bignum::from(state, 1);
    Integer* fix = pos->bit_xor(state, Bignum::from(state, FIXNUM_MAX-1));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(FIXNUM_MAX, fix->to_native());

    Bignum* neg = Bignum::from(state, 1);
    fix = neg->bit_xor(state, Bignum::from(state, FIXNUM_MIN-1));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(FIXNUM_MIN, fix->to_native());

    pos = Bignum::from(state, (FIXNUM_MAX+1)|FIXNUM_MAX);
    Integer* big = pos->bit_xor(state, Bignum::from(state, FIXNUM_MAX));

    TS_ASSERT(kind_of<Bignum>(big));
    TS_ASSERT_EQUALS(FIXNUM_MAX+1, big->to_native());

    neg = Bignum::from(state, FIXNUM_MAX);
    big = neg->bit_xor(state, Bignum::from(state, -1));

    TS_ASSERT(kind_of<Bignum>(big));
    TS_ASSERT_EQUALS(FIXNUM_MIN-1, big->to_native());
  }

  void test_bit_xor_with_fixnum() {
    Bignum* pos = Bignum::from(state, 1);
    Integer* fix = pos->bit_xor(state, Fixnum::from(FIXNUM_MAX-1));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(FIXNUM_MAX, fix->to_native());

    Bignum* neg = Bignum::from(state, FIXNUM_MIN-1);
    fix = neg->bit_xor(state, Fixnum::from(1));

    TS_ASSERT(kind_of<Fixnum>(fix));
    TS_ASSERT_EQUALS(FIXNUM_MIN, fix->to_native());
  }

  void test_gt() {
    TS_ASSERT_EQUALS(cTrue,  b1->gt(state, b2));
    TS_ASSERT_EQUALS(cFalse, b2->gt(state, b1));
    TS_ASSERT_EQUALS(cFalse, b1->gt(state, b1));
    TS_ASSERT_EQUALS(cTrue,  b1->gt(state, two));

    TS_ASSERT_EQUALS(cFalse,
        Bignum::from(state, (native_int)-3)->gt(state, Fixnum::from(-3)));
    TS_ASSERT_EQUALS(cFalse,
        Bignum::from(state, (native_int)-3)->gt(state, Fixnum::from(-2)));
    TS_ASSERT_EQUALS(cTrue,
        Bignum::from(state, (native_int)-3)->gt(state, Fixnum::from(-4)));

    TS_ASSERT_EQUALS(cFalse,
        Bignum::from(state, (native_int)-3)->gt(state, Float::create(state, -3.0)));
    TS_ASSERT_EQUALS(cFalse,
        Bignum::from(state, (native_int)-3)->gt(state, Float::create(state, -2.9)));
    TS_ASSERT_EQUALS(cTrue,
        Bignum::from(state, (native_int)-3)->gt(state, Float::create(state, -3.1)));
  }

  void test_ge() {
    TS_ASSERT_EQUALS(cTrue,  b1->ge(state, b2));
    TS_ASSERT_EQUALS(cFalse, b2->ge(state, b1));
    TS_ASSERT_EQUALS(cTrue,  b1->ge(state, b1));
    TS_ASSERT_EQUALS(cTrue,  b1->ge(state, two));

    TS_ASSERT_EQUALS(cTrue,
        Bignum::from(state, (native_int)-3)->ge(state, Fixnum::from(-3)));
    TS_ASSERT_EQUALS(cFalse,
        Bignum::from(state, (native_int)-3)->ge(state, Fixnum::from(-2)));
    TS_ASSERT_EQUALS(cTrue,
        Bignum::from(state, (native_int)-3)->ge(state, Fixnum::from(-4)));

    TS_ASSERT_EQUALS(cTrue,
        Bignum::from(state, (native_int)-3)->ge(state, Float::create(state, -3.0)));
    TS_ASSERT_EQUALS(cFalse,
        Bignum::from(state, (native_int)-3)->ge(state, Float::create(state, -2.9)));
    TS_ASSERT_EQUALS(cTrue,
        Bignum::from(state, (native_int)-3)->ge(state, Float::create(state, -3.1)));
  }

  void test_lt() {
    TS_ASSERT_EQUALS(cFalse, b1->lt(state, b2));
    TS_ASSERT_EQUALS(cTrue,  b2->lt(state, b1));
    TS_ASSERT_EQUALS(cFalse, b1->lt(state, b1));
    TS_ASSERT_EQUALS(cFalse, b1->lt(state, two));

    TS_ASSERT_EQUALS(cFalse,
        Bignum::from(state, (native_int)-3)->lt(state, Fixnum::from(-3)));
    TS_ASSERT_EQUALS(cTrue,
        Bignum::from(state, (native_int)-3)->lt(state, Fixnum::from(-2)));
    TS_ASSERT_EQUALS(cFalse,
        Bignum::from(state, (native_int)-3)->lt(state, Fixnum::from(-4)));

    TS_ASSERT_EQUALS(cFalse,
        Bignum::from(state, (native_int)-3)->lt(state, Float::create(state, -3.0)));
    TS_ASSERT_EQUALS(cTrue,
        Bignum::from(state, (native_int)-3)->lt(state, Float::create(state, -2.9)));
    TS_ASSERT_EQUALS(cFalse,
        Bignum::from(state, (native_int)-3)->lt(state, Float::create(state, -3.1)));
  }

  void test_le() {
    TS_ASSERT_EQUALS(cFalse, b1->le(state, b2));
    TS_ASSERT_EQUALS(cTrue,  b2->le(state, b1));
    TS_ASSERT_EQUALS(cTrue,  b1->le(state, b1));
    TS_ASSERT_EQUALS(cFalse, b1->le(state, two));

    TS_ASSERT_EQUALS(cTrue,
        Bignum::from(state, (native_int)-3)->le(state, Fixnum::from(-3)));
    TS_ASSERT_EQUALS(cTrue,
        Bignum::from(state, (native_int)-3)->le(state, Fixnum::from(-2)));
    TS_ASSERT_EQUALS(cFalse,
        Bignum::from(state, (native_int)-3)->le(state, Fixnum::from(-4)));

    TS_ASSERT_EQUALS(cTrue,
        Bignum::from(state, (native_int)-3)->le(state, Float::create(state, -3.0)));
    TS_ASSERT_EQUALS(cTrue,
        Bignum::from(state, (native_int)-3)->le(state, Float::create(state, -2.9)));
    TS_ASSERT_EQUALS(cFalse,
        Bignum::from(state, (native_int)-3)->le(state, Float::create(state, -3.1)));
  }

  void test_to_float() {
    check_float(b1->to_float(state), Float::create(state, 2147483647.0));
    check_float(b2->to_float(state), Float::create(state, 2147483646.0));

    Bignum *b3 = as<Bignum>(Fixnum::from(2)->pow(state, Fixnum::from(64)));
    check_float(b3->to_float(state), Float::create(state, 18446744073709551616.0));
    Bignum *b4 = as<Bignum>(Fixnum::from(2)->pow(state, Fixnum::from(65)));
    check_float(b4->to_float(state), Float::create(state, 36893488147419103232.0));
    Bignum *b5 = as<Bignum>(Fixnum::from(2)->pow(state, Fixnum::from(66)));
    check_float(b5->to_float(state), Float::create(state, 73786976294838206464.0));
  }

  /* Math.frexp(Float::MAX) => [1.0 - Float::EPSILON, 1024]. In other words,
   * the biggest floating point value that can be represented in IEEE 754 is
   * (2 ** 1024) * (1.0 - Float::EPSILON), so (2 ** 1024) is the smallest
   * mathematical integer (i.e. Bignum) that will cause a floating point
   * Infinity value.
   */
  void test_to_float_huge_positive() {
    Bignum* big = Bignum::from(state, 2);
    big = as<Bignum>(big->pow(state, Fixnum::from(1024)));

    Float* f = big->to_float(state);
    TS_ASSERT_EQUALS(f->fisinf(state), Fixnum::from(1));
  }

  void test_to_float_huge_negative() {
    Bignum* big = Bignum::from(state, 2);
    big = as<Bignum>(big->pow(state, Fixnum::from(1024)));
    big = as<Bignum>(big->mul(state, Fixnum::from(-1)));

    Float* f = big->to_float(state);
    TS_ASSERT_EQUALS(f->fisinf(state), Fixnum::from(-1));
  }

  void test_from_float() {
    double f = 2147483647.0;

    Object* s = Bignum::from_float(state, Float::create(state, f));
    TS_ASSERT_EQUALS(as<Integer>(s)->to_native(), (long)f);

    f = -2147483647.0;

    s = Bignum::from_float(state, Float::create(state, f));
    TS_ASSERT_EQUALS(as<Integer>(s)->to_native(), (long)f);
  }


  void test_from_double() { // TODO: wtf? this doesn't test bignum at all.
    Object* s = Bignum::from_double(state, 1.0);
    TS_ASSERT(s->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(s)->to_native(), 1.0);

    s = Bignum::from_double(state, -1);
    TS_ASSERT(s->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(s)->to_native(), -1);
  }

  void test_from_string_detect() {
    Object* b = Bignum::from_string_detect(state, "0x47");
    TS_ASSERT(b->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(b)->to_native(), 0x47);

    b = Bignum::from_string_detect(state, "0b1000111");
    TS_ASSERT(b->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(b)->to_native(), 0x47);

    b = Bignum::from_string_detect(state, "+0o107");
    TS_ASSERT(b->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(b)->to_native(), 0x47);

    b = Bignum::from_string_detect(state, "-0d71");
    TS_ASSERT(b->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(b)->to_native(), -0x47);

    b = Bignum::from_string_detect(state, "0107");
    TS_ASSERT(b->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(b)->to_native(), 0x47);
  }

  void test_from_string() {
    Object* b = Bignum::from_string(state, "47", 10);
    TS_ASSERT(b->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(b)->to_native(), 47);
  }

  void test_to_s() {
    /* Make sure we have a big enough bignum */
    char* buf = new char[2048];
    for(int i = 0; i < 2047; i++) {
      buf[i] = '1';
    }
    buf[2047] = '\0';
    Bignum* b = as<Bignum>(Bignum::from_string(state, buf, 10));
    String* s = b->to_s(state, Fixnum::from(10));

    TS_ASSERT_EQUALS(std::string(buf), s->c_str(state));
  }

  void test_size() {
    Object* s = b1->size(state);
    TS_ASSERT(s->fixnum_p());
    TS_ASSERT(as<Integer>(s)->to_native() > 0);
  }

  void test_hash_bignum() {
    hashval h = b1->hash_bignum(state);
    TS_ASSERT(h > 0);
  }

  void test_coerce_bignum() {
    Array* ary = b1->coerce(state, b2);
    Bignum* a = try_as<Bignum>(ary->get(state, 0));
    Bignum* b = try_as<Bignum>(ary->get(state, 1));

    TS_ASSERT_EQUALS(2, ary->size());
    TS_ASSERT(a);
    TS_ASSERT(b);
    TS_ASSERT_EQUALS(cTrue, b2->equal(state, a));
    TS_ASSERT_EQUALS(cTrue, b1->equal(state, b));
  }

  void test_coerce_fixnum() {
    Bignum* b = Bignum::create(state, two);
    Array* ary = b->coerce(state, two);

    Fixnum* e = try_as<Fixnum>(ary->get(state, 0));
    Fixnum* f = try_as<Fixnum>(ary->get(state, 1));

    TS_ASSERT_EQUALS(2, ary->size());
    TS_ASSERT(e);
    TS_ASSERT(f);
    TS_ASSERT_EQUALS(two, e);
    TS_ASSERT_EQUALS(cTrue, b->equal(state, f));

    Bignum* large = Bignum::from(state, 9223372036854775807LL);
    ary = large->coerce(state, two);
    Bignum* c = try_as<Bignum>(ary->get(state, 0));
    Bignum* d = try_as<Bignum>(ary->get(state, 1));

    TS_ASSERT_EQUALS(2, ary->size());
    TS_ASSERT(c);
    TS_ASSERT(d);
    TS_ASSERT_EQUALS(cTrue, Bignum::create(state, two)->equal(state, c));
    TS_ASSERT_EQUALS(cTrue, large->equal(state, d));
  }
};
