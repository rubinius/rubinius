#include "vm/test/test.hpp"

#include "builtin/float.hpp"

class TestFloat : public CxxTest::TestSuite, public VMTest {
public:

  double TOLERANCE;

  void setUp() {
    create();
    TOLERANCE = 0.00003;
  }

  void tearDown() {
    destroy();
  }

  void check_float(Float* f, Float* g) {
    TS_ASSERT_DELTA(f->val, g->val, TOLERANCE);
  }

  void test_create() {
    Float* flt = Float::create(state, 1.0);
    TS_ASSERT_EQUALS(flt->val, 1.0);
  }

  void test_coerce() {
    Object* o = Fixnum::from(5432);
    Float* coerced = Float::coerce(state, o);
    TS_ASSERT(kind_of<Float>(coerced));

    Bignum* bn = Bignum::from(state, (native_int)2147483647);
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
    a = f->add(state, Fixnum::from(10));
    check_float(a, Float::create(state, 10.2));

    f = Float::create(state, 0.2);
    a = f->add(state, Bignum::from(state, FIXNUM_MAX + 10));
    check_float(a, Float::create(state, (double)FIXNUM_MAX + 10.2));
  }

  void test_sub() {
    Float* f = Float::create(state, 0.2);
    Float* a = f->sub(state, Float::create(state, 0.4));
    check_float(a, Float::create(state, -0.2));

    f = Float::create(state, 0.2);
    a = f->sub(state, Fixnum::from(10));
    check_float(a, Float::create(state, -9.8));

    f = Float::create(state, 0.2);
    a = f->sub(state, Bignum::from(state, FIXNUM_MAX + 10));
    check_float(a, Float::create(state, (double)-FIXNUM_MAX - 9.8));
  }

  void test_mul() {
    Float* f = Float::create(state, 0.2);
    Float* a = f->mul(state, Float::create(state, 0.4));
    check_float(a, Float::create(state, 0.08));

    f = Float::create(state, 0.2);
    a = f->mul(state, Fixnum::from(10));
    check_float(a, Float::create(state, 2.0));

    f = Float::create(state, 0.2);
    a = f->mul(state, Bignum::from(state, FIXNUM_MAX + 10));
    check_float(a, Float::create(state, ((double)FIXNUM_MAX + 10) * 0.2));
  }

  void test_pow() {
    Float* f = Float::create(state, 0.2);
    Float* a = f->fpow(state, Float::create(state, 0.4));
    check_float(a, Float::create(state, 0.525305560880753));

    f = Float::create(state, 1.2);
    a = f->fpow(state, Fixnum::from(10));
    check_float(a, Float::create(state, 6.1917364224));
  }

  void test_div() {
    Float* f = Float::create(state, 0.2);
    Float* a = f->div(state, Float::create(state, 0.4));
    check_float(a, Float::create(state, 0.5));

    f = Float::create(state, 0.2);
    a = f->div(state, Fixnum::from(10));
    check_float(a, Float::create(state, 0.02));

    f = Float::create(state, (native_int)100000);
    a = f->div(state, Bignum::from(state, FIXNUM_MAX + 10));
    check_float(a, Float::create(state, 100000 / ((double)FIXNUM_MAX + 10)));
  }

  void test_mod() {
    Float* f = Float::create(state, 2.2);
    Float* a = f->mod(state, Float::create(state, 0.5));
    check_float(a, Float::create(state, 0.2));

    f = Float::create(state, -2.2);
    a = f->mod(state, Float::create(state, 0.5));
    check_float(a, Float::create(state, 0.3));

    f = Float::create(state, 2.2);
    a = f->mod(state, Float::create(state, -0.5));
    check_float(a, Float::create(state, -0.3));

    f = Float::create(state, -2.2);
    a = f->mod(state, Float::create(state, -0.5));
    check_float(a, Float::create(state, -0.2));

    f = Float::create(state, 2.2);
    a = f->mod(state, Fixnum::from(1));
    check_float(a, Float::create(state, 0.2));
  }

  void test_divmod() {
    Float*  f  = Float::create(state, 2.2);
    Array*  a  = f->divmod(state, Float::create(state, 0.5));
    Object* o1 = a->get(state, 0);
    Float*  o2 = as<Float>(a->get(state, 1));

    TS_ASSERT(o1->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(o1)->to_native(), 4);
    check_float(o2, Float::create(state, 0.2));

    f = Float::create(state, -2.2);
    a = f->divmod(state, Float::create(state, 0.5));
    o1 = a->get(state, 0);
    o2 = as<Float>(a->get(state, 1));

    TS_ASSERT(o1->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(o1)->to_native(), -5);
    check_float(o2, Float::create(state, 0.3));

    f = Float::create(state, 2.2);
    a = f->divmod(state, Float::create(state, -0.5));
    o1 = a->get(state, 0);
    o2 = as<Float>(a->get(state, 1));

    TS_ASSERT(o1->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(o1)->to_native(), -5);
    check_float(o2, Float::create(state, -0.3));

    f = Float::create(state, -2.2);
    a = f->divmod(state, Float::create(state, -0.5));
    o1 = a->get(state, 0);
    o2 = as<Float>(a->get(state, 1));

    TS_ASSERT(o1->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(o1)->to_native(), 4);
    check_float(o2, Float::create(state, -0.2));

    f = Float::create(state, -2.2);
    a = f->divmod(state, Fixnum::from(2));
    o1 = a->get(state, 0);
    o2 = as<Float>(a->get(state, 1));

    TS_ASSERT(o1->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(o1)->to_native(), -2);
    check_float(o2, Float::create(state, 1.8));
  }

  void test_neg() {
    Float* f = Float::create(state, 0.2);
    Float* a = f->neg(state);
    check_float(a, Float::create(state, -0.2));
  }

  void test_equal() {
    TS_ASSERT_EQUALS(Float::create(state, 0.2)->equal(state, Float::create(state, 0.2)), Qtrue);
    TS_ASSERT_EQUALS(Float::create(state, 0.2)->equal(state, Float::create(state, 0.3)), Qfalse);

    TS_ASSERT_EQUALS(Float::create(state, 2.0)->equal(state, Fixnum::from(2)), Qtrue);
    TS_ASSERT_EQUALS(Float::create(state, 2.1)->equal(state, Fixnum::from(2)), Qfalse);
  }

  void test_eql() {
    TS_ASSERT_EQUALS(Float::create(state, 0.2)->eql(state, Float::create(state, 0.2)), Qtrue);
    TS_ASSERT_EQUALS(Float::create(state, 0.2)->eql(state, Float::create(state, 0.3)), Qfalse);

    TS_ASSERT_EQUALS(Float::create(state, 2.0)->eql(state, Fixnum::from(2)), Qfalse);
    TS_ASSERT_EQUALS(Float::create(state, 2.1)->eql(state, Fixnum::from(2)), Qfalse);
  }

  void test_compare() {
    TS_ASSERT_EQUALS(Float::create(state, 0.2)->compare(state, Float::create(state, 0.1)), Fixnum::from( 1));
    TS_ASSERT_EQUALS(Float::create(state, 0.2)->compare(state, Float::create(state, 0.2)), Fixnum::from( 0));
    TS_ASSERT_EQUALS(Float::create(state, 0.2)->compare(state, Float::create(state, 0.3)), Fixnum::from(-1));

    TS_ASSERT_EQUALS(Float::create(state, 1.9)->compare(state, Fixnum::from(2)), Fixnum::from(-1));
    TS_ASSERT_EQUALS(Float::create(state, 2.0)->compare(state, Fixnum::from(2)), Fixnum::from( 0));
    TS_ASSERT_EQUALS(Float::create(state, 2.1)->compare(state, Fixnum::from(2)), Fixnum::from( 1));
  }

  void test_gt() {
    TS_ASSERT_EQUALS(Float::create(state, 0.2)->gt(state, Float::create(state, 0.1)), Qtrue);
    TS_ASSERT_EQUALS(Float::create(state, 0.2)->gt(state, Float::create(state, 0.2)), Qfalse);
    TS_ASSERT_EQUALS(Float::create(state, 0.2)->gt(state, Float::create(state, 0.3)), Qfalse);

    TS_ASSERT_EQUALS(Float::create(state, 1.9)->gt(state, Fixnum::from(2)), Qfalse);
    TS_ASSERT_EQUALS(Float::create(state, 2.0)->gt(state, Fixnum::from(2)), Qfalse);
    TS_ASSERT_EQUALS(Float::create(state, 2.1)->gt(state, Fixnum::from(2)), Qtrue);
  }

  void test_ge() {
    TS_ASSERT_EQUALS(Float::create(state, 0.2)->ge(state, Float::create(state, 0.1)), Qtrue);
    TS_ASSERT_EQUALS(Float::create(state, 0.2)->ge(state, Float::create(state, 0.2)), Qtrue);
    TS_ASSERT_EQUALS(Float::create(state, 0.2)->ge(state, Float::create(state, 0.3)), Qfalse);

    TS_ASSERT_EQUALS(Float::create(state, 1.9)->ge(state, Fixnum::from(2)), Qfalse);
    TS_ASSERT_EQUALS(Float::create(state, 2.0)->ge(state, Fixnum::from(2)), Qtrue);
    TS_ASSERT_EQUALS(Float::create(state, 2.1)->ge(state, Fixnum::from(2)), Qtrue);
  }

  void test_lt() {
    TS_ASSERT_EQUALS(Float::create(state, 0.2)->lt(state, Float::create(state, 0.1)), Qfalse);
    TS_ASSERT_EQUALS(Float::create(state, 0.2)->lt(state, Float::create(state, 0.2)), Qfalse);
    TS_ASSERT_EQUALS(Float::create(state, 0.2)->lt(state, Float::create(state, 0.3)), Qtrue);

    TS_ASSERT_EQUALS(Float::create(state, 1.9)->lt(state, Fixnum::from(2)), Qtrue);
    TS_ASSERT_EQUALS(Float::create(state, 2.0)->lt(state, Fixnum::from(2)), Qfalse);
    TS_ASSERT_EQUALS(Float::create(state, 2.1)->lt(state, Fixnum::from(2)), Qfalse);
  }

  void test_le() {
    TS_ASSERT_EQUALS(Float::create(state, 0.2)->le(state, Float::create(state, 0.1)), Qfalse);
    TS_ASSERT_EQUALS(Float::create(state, 0.2)->le(state, Float::create(state, 0.2)), Qtrue);
    TS_ASSERT_EQUALS(Float::create(state, 0.2)->le(state, Float::create(state, 0.3)), Qtrue);

    TS_ASSERT_EQUALS(Float::create(state, 1.9)->le(state, Fixnum::from(2)), Qtrue);
    TS_ASSERT_EQUALS(Float::create(state, 2.0)->le(state, Fixnum::from(2)), Qtrue);
    TS_ASSERT_EQUALS(Float::create(state, 2.1)->le(state, Fixnum::from(2)), Qfalse);
  }

  void test_isinf() {
    TS_ASSERT_EQUALS(Float::create(state, 2.0)->fisinf(state), Qnil);
    TS_ASSERT_EQUALS(Float::create(state, 2.0)->div(state, Float::create(state, 0.0))->fisinf(state), Fixnum::from(1));
    TS_ASSERT_EQUALS(Float::create(state, -2.0)->div(state, Float::create(state, 0.0))->fisinf(state), Fixnum::from(-1));
  }

  void test_isnan() {
    TS_ASSERT_EQUALS(Float::create(state, 2.0)->fisnan(state), Qfalse);
    TS_ASSERT_EQUALS(Float::create(state, 0.0)->div(state, Float::create(state, 0.0))->fisnan(state), Qtrue);
  }

  void test_round() {
    TS_ASSERT_EQUALS(Float::create(state, 1.5)->fround(state), Fixnum::from(2));
    TS_ASSERT_EQUALS(Float::create(state, 1.4)->fround(state), Fixnum::from(1));
    TS_ASSERT_EQUALS(Float::create(state, -1.4)->fround(state), Fixnum::from(-1));
    TS_ASSERT_EQUALS(Float::create(state, -1.5)->fround(state), Fixnum::from(-2));

    Float* zero = Float::create(state,  0.0);
    TS_ASSERT_THROWS_ASSERT(zero->div(state, zero)->fround(state), const RubyException &e,
                            TS_ASSERT(Exception::float_domain_error_p(state, e.exception)));
    Float* one = Float::create(state,  1.0);
    TS_ASSERT_THROWS_ASSERT(one->div(state, zero)->fround(state), const RubyException &e,
                            TS_ASSERT(Exception::float_domain_error_p(state, e.exception)));
    Float* neg_one = Float::create(state,  1.0);
    TS_ASSERT_THROWS_ASSERT(neg_one->div(state, zero)->fround(state), const RubyException &e,
                            TS_ASSERT(Exception::float_domain_error_p(state, e.exception)));
  }

  void test_to_i() {
    TS_ASSERT_EQUALS(Float::create(state, 1.9)->to_i(state), Fixnum::from(1));
    TS_ASSERT_EQUALS(Float::create(state, 1.1)->to_i(state), Fixnum::from(1));
    TS_ASSERT_EQUALS(Float::create(state, 0.9)->to_i(state), Fixnum::from(0));
    TS_ASSERT_EQUALS(Float::create(state, -0.9)->to_i(state), Fixnum::from(0));
    TS_ASSERT_EQUALS(Float::create(state, -1.1)->to_i(state), Fixnum::from(-1));
    TS_ASSERT_EQUALS(Float::create(state, -1.9)->to_i(state), Fixnum::from(-1));

    Float* zero = Float::create(state,  0.0);
    TS_ASSERT_THROWS_ASSERT(zero->div(state, zero)->to_i(state), const RubyException &e,
                            TS_ASSERT(Exception::float_domain_error_p(state, e.exception)));
    Float* one = Float::create(state,  1.0);
    TS_ASSERT_THROWS_ASSERT(one->div(state, zero)->to_i(state), const RubyException &e,
                            TS_ASSERT(Exception::float_domain_error_p(state, e.exception)));
    Float* neg_one = Float::create(state,  1.0);
    TS_ASSERT_THROWS_ASSERT(neg_one->div(state, zero)->to_i(state), const RubyException &e,
                            TS_ASSERT(Exception::float_domain_error_p(state, e.exception)));
  }

  void test_into_string() {
    Float* flt = Float::create(state, 1.0);
    char buf[10];
    flt->into_string(state, buf,10);
    TS_ASSERT(strcmp(buf, "+1.000000") == 0);
  }

  void test_to_s_formatted() {
    Float* f = Float::create(state, 3.14159);
    String* format = String::create(state, "%#.15g");

    String* s = f->to_s_formatted(state, format);

    TS_ASSERT_SAME_DATA("3.14159000000000", s->c_str(), 16);
    TS_ASSERT_EQUALS(16U, s->size());

    format = String::create(state, "%#.1280g");
    TS_ASSERT_THROWS_ASSERT(f->to_s_formatted(state, format), const RubyException &e,
                            TS_ASSERT(Exception::argument_error_p(state, e.exception)));
  }
};

