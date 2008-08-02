#include "builtin/bignum.hpp"
#include "builtin/float.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestBignum : public CxxTest::TestSuite {
  public:

  VM* state;
  Bignum* b1;
  Bignum* b2;
  FIXNUM two;
  double TOLERANCE;

  void setUp() {
    state = new VM(1024);
    b1 = Bignum::create(state, (native_int)2147483647);
    b2 = Bignum::create(state, (native_int)2147483646);
    two = Object::i2n(2);
    TOLERANCE = 0.00003;
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

  void check_bignum(OBJECT big, const char* val) {
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

  void check_float(Float* f, Float* g) {
    TS_ASSERT_RELATION(std::greater<double>, f->val + TOLERANCE, g->val);
    TS_ASSERT_RELATION(std::greater<double>, f->val, g->val - TOLERANCE);
    TS_ASSERT_RELATION(std::greater<double>, g->val + TOLERANCE, f->val);
    TS_ASSERT_RELATION(std::greater<double>, g->val, f->val - TOLERANCE);
  }

  void test_add() {
    check_bignum(b1->add(state, b1), "4294967294");
    check_bignum(b1->add(state, Object::i2n(1)), "2147483648");
  }

  void test_add_with_positive_fixnum() {
    check_bignum(b1->add(state, Object::i2n(100)), "2147483747");
  }

  void test_add_with_negative_fixnum() {
    check_bignum(b1->add(state, Object::i2n(-100)), "2147483547");
  }

  void test_add_with_float() {
    Float* f = Float::create(state, 0.2);
    Float* a = Bignum::create(state, FIXNUM_MAX + 10)->add(state, f);
    check_float(a, Float::create(state, (double)FIXNUM_MAX + 10.2));
  }

  void test_sub() {
    check_bignum(b1->sub(state, b1), "0");
    check_bignum(b1->sub(state, Object::i2n(1)), "2147483646");
  }

  void test_sub_with_positive_fixnum() {
    check_bignum(b1->sub(state, Object::i2n(100)), "2147483547");
  }

  void test_sub_with_negative_fixnum() {
    check_bignum(b1->sub(state, Object::i2n(-100)), "2147483747");
  }

  void test_sub_with_float() {
    Float* f = Float::create(state, 0.2);
    Float* a = Bignum::create(state, FIXNUM_MAX + 10)->sub(state, f);
    check_float(a, Float::create(state, (double)FIXNUM_MAX + 9.8));
  }

  void test_mul() {
    check_bignum(b1->mul(state, b1), "4611686014132420609");
    check_bignum(b1->mul(state, Object::i2n(2)), "4294967294");
    check_bignum(b1->mul(state, Object::i2n(1)), "2147483647");
  }

  void test_mul_with_positive_fixnum() {
    check_bignum(b1->mul(state, Object::i2n(100)), "214748364700");
  }

  void test_mul_with_negative_fixnum() {
    check_bignum(b1->mul(state, Object::i2n(-100)), "-214748364700");
  }

  void test_mul_with_float() {
    Float* f = Float::create(state, 0.2);
    Float* a = Bignum::create(state, FIXNUM_MAX + 10)->mul(state, f);
    check_float(a, Float::create(state, ((double)FIXNUM_MAX + 10) * 0.2));
  }

  void test_div() {
    INTEGER div = b1->div(state, b1);
    TS_ASSERT(div->fixnum_p());
    TS_ASSERT_EQUALS(div->n2i(), 1);

    TS_ASSERT_THROWS( b1->div(state, Bignum::create(state, 0)), const ZeroDivisionError &);
  }

  void test_div_with_fixnum() {
    INTEGER div = b1->div(state, two);
    check_bignum(div,"1073741823");

    TS_ASSERT_THROWS( b1->div(state, Object::i2n(0)), const ZeroDivisionError &);
  }

  void test_div_with_float() {
    Float* div = b1->div(state, Float::create(state, 1.5));
    check_float(div, Float::create(state, 1431655764.6666667));
  }

  void test_divmod() {
    Array* ary1 = b1->divmod(state, b1);
    Object* o1 = ary1->get(state, 0);
    Object* o2 = ary1->get(state, 1);
    TS_ASSERT(o1->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(o1)->n2i(), 1);
    TS_ASSERT(o2->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(o2)->n2i(), 0);
    
    ary1 = b1->divmod(state, two);
    o1 = ary1->get(state, 0);
    o2 = ary1->get(state, 1);
    check_bignum(o1,"1073741823");
    check_bignum(o2, "1");
    
    Bignum* nbn1 = Bignum::create(state, (native_int)-2147483647);
    
    ary1 = b1->divmod(state, nbn1);
    o1 = ary1->get(state, 0);
    o2 = ary1->get(state, 1);
    check_bignum(o1,"-1");
    check_bignum(o2, "0");
    
    ary1 = nbn1->divmod(state, b1);
    o1 = ary1->get(state, 0);
    o2 = ary1->get(state, 1);
    check_bignum(o1,"-1");
    check_bignum(o2, "0");
    
    ary1 = nbn1->divmod(state, nbn1);
    o1 = ary1->get(state, 0);
    o2 = ary1->get(state, 1);
    check_bignum(o1,"1");
    check_bignum(o2, "0");

    ary1 = nbn1->divmod(state, Bignum::create(state, (native_int)89478485));
    o1 = ary1->get(state, 0);
    o2 = ary1->get(state, 1);
    check_bignum(o1,"-25");
    check_bignum(o2, "89478478");
    
    ary1 = nbn1->divmod(state, Object::i2n(89478485));
    o1 = ary1->get(state, 0);
    o2 = ary1->get(state, 1);
    check_bignum(o1,"-25");
    check_bignum(o2, "89478478");
    
    ary1 = b1->divmod(state, Object::i2n(-89478485));
    o1 = ary1->get(state, 0);
    o2 = ary1->get(state, 1);
    check_bignum(o1,"-25");
    check_bignum(o2, "-89478478");

    ary1 = b1->divmod(state, Float::create(state, 2.5));
    o1 = ary1->get(state, 0);
    o2 = ary1->get(state, 1);
    check_bignum(o1,"858993458");
    check_float(as<Float>(o2), Float::create(state, 2.0));
  }

  void test_mod() {
    INTEGER mod = b1->mod(state, Object::i2n(-89478485));
    check_bignum(mod, "-89478478");
    mod = b1->mod(state, Bignum::create(state, (native_int)-2147483645));
    check_bignum(mod, "-2147483643");
    Float* f = b1->mod(state, Float::create(state, -21483645.2));
    check_float(f, Float::create(state, -880872.999999925));
  }

  void test_left_shift() {
    INTEGER shifted = b1->left_shift(state, Object::i2n(3));
    check_bignum(shifted, "17179869176");

    shifted = b1->left_shift(state, Object::i2n(-3));
    check_bignum(shifted, "268435455");

    Bignum* nbn1 = Bignum::create(state, (native_int)-2147483647);
    shifted = nbn1->left_shift(state, Object::i2n(3));
    check_bignum(shifted, "-17179869176");
  }

  void test_right_shift() {
    INTEGER shifted = b1->right_shift(state, Object::i2n(3));
    check_bignum(shifted, "268435455");

    shifted = b1->right_shift(state, Object::i2n(-3));
    check_bignum(shifted, "17179869176");

    shifted = b1->right_shift(state, Object::i2n(1048576));
    check_bignum(shifted, "0");

    shifted = b1->right_shift(state, Object::i2n(0));
    check_bignum(shifted, "2147483647");

    Bignum* nbn1 = Bignum::create(state, (native_int)-2147483647);
    shifted = nbn1->right_shift(state, Object::i2n(3));
    check_bignum(shifted, "-268435456");

    shifted = nbn1->right_shift(state, Object::i2n(1048576));
    check_bignum(shifted, "-1");
  }

  void test_equal() {
    TS_ASSERT_EQUALS(b1->equal(state, b1), Qtrue);
    TS_ASSERT_EQUALS(b1->equal(state, b2), Qfalse);

    Bignum* b = Bignum::create(state, 2);
    TS_ASSERT_EQUALS(b->equal(state, Object::i2n(2)), Qtrue);
    TS_ASSERT_EQUALS(b->equal(state, Object::i2n(1)), Qfalse);

    b = Bignum::create(state, -2);
    TS_ASSERT_EQUALS(b->equal(state, Object::i2n(-2)), Qtrue);
    TS_ASSERT_EQUALS(b->equal(state, Object::i2n(1)), Qfalse);

    TS_ASSERT_EQUALS(b->equal(state, Float::create(state, -2.0)), Qtrue);
    TS_ASSERT_EQUALS(b->equal(state, Float::create(state, 1.9)), Qfalse);
  }

  void test_compare() {
    TS_ASSERT_EQUALS(b1->compare(state, b2), Object::i2n(1));
    TS_ASSERT_EQUALS(b2->compare(state, b1), Object::i2n(-1));
    TS_ASSERT_EQUALS(b1->compare(state, b1), Object::i2n(0));

    TS_ASSERT_EQUALS(b1->compare(state, Object::i2n(0)), Object::i2n(1));
  }

  void test_compare_with_fixnum() {
    TS_ASSERT_EQUALS(Bignum::create(state, 3)->compare(state, Object::i2n(3)), Object::i2n(0));
    TS_ASSERT_EQUALS(Bignum::create(state, 3)->compare(state, Object::i2n(2)), Object::i2n(1));
    TS_ASSERT_EQUALS(Bignum::create(state, 3)->compare(state, Object::i2n(4)), Object::i2n(-1));

    TS_ASSERT_EQUALS(Bignum::create(state, -3)->compare(state, Object::i2n(-3)), Object::i2n(0));
    TS_ASSERT_EQUALS(Bignum::create(state, -3)->compare(state, Object::i2n(-2)), Object::i2n(-1));
    TS_ASSERT_EQUALS(Bignum::create(state, -3)->compare(state, Object::i2n(-4)), Object::i2n(1));
  }

  void test_compare_with_float() {
    TS_ASSERT_EQUALS(Bignum::create(state, 3)->compare(state, Float::create(state, 3.0)), Object::i2n(0));
    TS_ASSERT_EQUALS(Bignum::create(state, 3)->compare(state, Float::create(state, 2.9)), Object::i2n(1));
    TS_ASSERT_EQUALS(Bignum::create(state, 3)->compare(state, Float::create(state, 3.1)), Object::i2n(-1));

    TS_ASSERT_EQUALS(Bignum::create(state, -3)->compare(state, Float::create(state, -3.0)), Object::i2n(0));
    TS_ASSERT_EQUALS(Bignum::create(state, -3)->compare(state, Float::create(state, -2.9)), Object::i2n(-1));
    TS_ASSERT_EQUALS(Bignum::create(state, -3)->compare(state, Float::create(state, -3.1)), Object::i2n(1));
  }

  void test_neg() {
    INTEGER negated = b1->neg(state);
    check_bignum(negated, "-2147483647");
  }

  void test_invert() {
    INTEGER inverted = b1->invert(state);
    check_bignum(inverted, "-2147483648");
  }

  void test_bit_and() {
    Bignum* bn1 = Bignum::create(state, (native_int)2109047783);
    Bignum* nbn1 = Bignum::create(state, (native_int)-2109047783);
    check_bignum(bn1->bit_and(state, Object::i2n(-23472)), "2109046848");
    check_bignum(nbn1->bit_and(state, Object::i2n(23472056)), "4334616");

    Bignum* bn2 = Bignum::create(state, (native_int)23472);
    Bignum* nbn2 = Bignum::create(state, (native_int)-23472);
    check_bignum(bn2->bit_and(state, Object::i2n(-21090)), "2448");
    check_bignum(nbn2->bit_and(state, Object::i2n(2109047)), "2106448");

    check_bignum(bn2->bit_and(state, Float::create(state, -21090.2)), "2448");
    check_bignum(nbn2->bit_and(state, Float::create(state, 2109047.7)), "2106448");
  }

  void test_bit_or() {
    Bignum* bn1 = Bignum::create(state, (native_int)2109047783);
    Bignum* nbn1 = Bignum::create(state, (native_int)-2109047783);
    check_bignum(bn1->bit_or(state, Object::i2n(-23472)), "-22537");
    check_bignum(nbn1->bit_or(state, Object::i2n(23472056)), "-2089910343");

    Bignum* bn2 = Bignum::create(state, (native_int)23472);
    Bignum* nbn2 = Bignum::create(state, (native_int)-23472);
    check_bignum(bn2->bit_or(state, Object::i2n(-21090)), "-66");
    check_bignum(nbn2->bit_or(state, Object::i2n(2109047)), "-20873");

    check_bignum(bn2->bit_or(state, Float::create(state, -21090.1)), "-66");
    check_bignum(nbn2->bit_or(state, Float::create(state, 2109047.9)), "-20873");
  }

  void test_bit_xor() {
    Bignum* bn1 = Bignum::create(state, (native_int)2109047783);
    Bignum* nbn1 = Bignum::create(state, (native_int)-2109047783);
    check_bignum(bn1->bit_xor(state, Object::i2n(-23472)), "-2109069385");
    check_bignum(nbn1->bit_xor(state, Object::i2n(23472056)), "-2094244959");

    Bignum* bn2 = Bignum::create(state, (native_int)23472);
    Bignum* nbn2 = Bignum::create(state, (native_int)-23472);
    check_bignum(bn2->bit_xor(state, Object::i2n(-21090)), "-2514");
    check_bignum(nbn2->bit_xor(state, Object::i2n(2109047)), "-2127321");

    check_bignum(bn2->bit_xor(state, Float::create(state, -21090.1)), "-2514");
    check_bignum(nbn2->bit_xor(state, Float::create(state, 2109047.9)), "-2127321");
  }

  void test_gt() {
    TS_ASSERT_EQUALS(b1->gt(state, b2), Qtrue);
    TS_ASSERT_EQUALS(b2->gt(state, b1), Qfalse);
    TS_ASSERT_EQUALS(b1->gt(state, b1), Qfalse);
    TS_ASSERT_EQUALS(b1->gt(state, two), Qtrue);

    TS_ASSERT_EQUALS(Bignum::create(state, -3)->gt(state, Object::i2n(-3)), Qfalse);
    TS_ASSERT_EQUALS(Bignum::create(state, -3)->gt(state, Object::i2n(-2)), Qfalse);
    TS_ASSERT_EQUALS(Bignum::create(state, -3)->gt(state, Object::i2n(-4)), Qtrue);

    TS_ASSERT_EQUALS(Bignum::create(state, -3)->gt(state, Float::create(state, -3.0)), Qfalse);
    TS_ASSERT_EQUALS(Bignum::create(state, -3)->gt(state, Float::create(state, -2.9)), Qfalse);
    TS_ASSERT_EQUALS(Bignum::create(state, -3)->gt(state, Float::create(state, -3.1)), Qtrue);
  }

  void test_ge() {
    TS_ASSERT_EQUALS(b1->ge(state, b2), Qtrue);
    TS_ASSERT_EQUALS(b2->ge(state, b1), Qfalse);
    TS_ASSERT_EQUALS(b1->ge(state, b1), Qtrue);
    TS_ASSERT_EQUALS(b1->ge(state, two), Qtrue);

    TS_ASSERT_EQUALS(Bignum::create(state, -3)->ge(state, Object::i2n(-3)), Qtrue);
    TS_ASSERT_EQUALS(Bignum::create(state, -3)->ge(state, Object::i2n(-2)), Qfalse);
    TS_ASSERT_EQUALS(Bignum::create(state, -3)->ge(state, Object::i2n(-4)), Qtrue);

    TS_ASSERT_EQUALS(Bignum::create(state, -3)->ge(state, Float::create(state, -3.0)), Qtrue);
    TS_ASSERT_EQUALS(Bignum::create(state, -3)->ge(state, Float::create(state, -2.9)), Qfalse);
    TS_ASSERT_EQUALS(Bignum::create(state, -3)->ge(state, Float::create(state, -3.1)), Qtrue);
  }

  void test_lt() {
    TS_ASSERT_EQUALS(b1->lt(state, b2), Qfalse);
    TS_ASSERT_EQUALS(b2->lt(state, b1), Qtrue);
    TS_ASSERT_EQUALS(b1->lt(state, b1), Qfalse);
    TS_ASSERT_EQUALS(b1->lt(state, two), Qfalse);

    TS_ASSERT_EQUALS(Bignum::create(state, -3)->lt(state, Object::i2n(-3)), Qfalse);
    TS_ASSERT_EQUALS(Bignum::create(state, -3)->lt(state, Object::i2n(-2)), Qtrue);
    TS_ASSERT_EQUALS(Bignum::create(state, -3)->lt(state, Object::i2n(-4)), Qfalse);

    TS_ASSERT_EQUALS(Bignum::create(state, -3)->lt(state, Float::create(state, -3.0)), Qfalse);
    TS_ASSERT_EQUALS(Bignum::create(state, -3)->lt(state, Float::create(state, -2.9)), Qtrue);
    TS_ASSERT_EQUALS(Bignum::create(state, -3)->lt(state, Float::create(state, -3.1)), Qfalse);
  }

  void test_le() {
    TS_ASSERT_EQUALS(b1->le(state, b2), Qfalse);
    TS_ASSERT_EQUALS(b2->le(state, b1), Qtrue);
    TS_ASSERT_EQUALS(b1->le(state, b1), Qtrue);
    TS_ASSERT_EQUALS(b1->le(state, two), Qfalse);

    TS_ASSERT_EQUALS(Bignum::create(state, -3)->le(state, Object::i2n(-3)), Qtrue);
    TS_ASSERT_EQUALS(Bignum::create(state, -3)->le(state, Object::i2n(-2)), Qtrue);
    TS_ASSERT_EQUALS(Bignum::create(state, -3)->le(state, Object::i2n(-4)), Qfalse);

    TS_ASSERT_EQUALS(Bignum::create(state, -3)->le(state, Float::create(state, -3.0)), Qtrue);
    TS_ASSERT_EQUALS(Bignum::create(state, -3)->le(state, Float::create(state, -2.9)), Qtrue);
    TS_ASSERT_EQUALS(Bignum::create(state, -3)->le(state, Float::create(state, -3.1)), Qfalse);
  }

  void test_to_f() {
    check_float(b1->to_f(state), Float::create(state, 2147483647.0));
  }

  void test_from_double_external() {
    double f = 2147483647.0;

    OBJECT s = Bignum::from_double(state, Float::create(state, f));
    TS_ASSERT_EQUALS(as<Integer>(s)->n2i(), (long)f);

    f = -2147483647.0;

    s = Bignum::from_double(state, Float::create(state, f));
    TS_ASSERT_EQUALS(as<Integer>(s)->n2i(), (long)f);
  }


  void test_from_double_internal() { // TODO: wtf? this doesn't test bignum at all.
    OBJECT s = Bignum::from_double(state, 1.0);
    TS_ASSERT(s->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(s)->n2i(), 1.0);

    s = Bignum::from_double(state, -1);
    TS_ASSERT(s->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(s)->n2i(), -1);
  }

  void test_from_string_detect() {
    OBJECT b = Bignum::from_string_detect(state, "0x47");
    TS_ASSERT(b->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(b)->n2i(), 0x47);

    b = Bignum::from_string_detect(state, "0b1000111");
    TS_ASSERT(b->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(b)->n2i(), 0x47);

    b = Bignum::from_string_detect(state, "+0o107");
    TS_ASSERT(b->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(b)->n2i(), 0x47);

    b = Bignum::from_string_detect(state, "-0d71");
    TS_ASSERT(b->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(b)->n2i(), -0x47);

    b = Bignum::from_string_detect(state, "0107");
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
