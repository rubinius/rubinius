#include <sstream>

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/encoding.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/string.hpp"
#include "builtin/bytearray.hpp"
#include "configuration.hpp"
#include "missing/math.h"
#include "object_utils.hpp"
#include "ontology.hpp"
#include "version.h"

#define BASIC_CLASS(blah) G(blah)
#define NEW_STRUCT(obj, str, kls, kind) \
  obj = (typeof(obj))Bignum::create(state); \
  str = (kind *)(obj->mp_val())
#define DATA_STRUCT(obj, type) ((type)(obj->mp_val()))

#define NMP mp_int *n; Bignum* n_obj; \
  NEW_STRUCT(n_obj, n, BASIC_CLASS(bignum), mp_int);

#define MMP mp_int *m; Bignum* m_obj; \
  NEW_STRUCT(m_obj, m, BASIC_CLASS(bignum), mp_int);


#define BDIGIT_DBL long long
#define DIGIT_RADIX (1UL << DIGIT_BIT)

#define XST ((void*)state)

namespace rubinius {

  /*
   * LibTomMath actually stores stuff internally in longs.
   * The problem with it's API is that it's int based, so
   * this will create problems on 64 bit platforms if
   * everything is cut down to 32 bits. Hence the existence
   * of the mp_set_long, mp_init_set_long and mp_get_long
   * functions here.
   */
  static int mp_set_long MPA(mp_int* a, unsigned long b) {
    mp_zero(a);

    while(b > MP_DIGIT_MAX) {
      a->dp[0] |= (b >> DIGIT_BIT);
      a->used += 1;
      int err = mp_mul_2d(MPST, a, DIGIT_BIT, a);
      if(err != MP_OKAY) {
        return err;
      }

      b &= MP_MASK;
    }

    a->dp[0] |= b;
    a->used += 1;

    mp_clamp(a);
    return MP_OKAY;
  }

  static unsigned long mp_get_long(mp_int* a) {
      int i;
      unsigned long res;

      if(a->used == 0) return 0;

      /* get number of digits of the lsb we have to read */
      i = MIN(a->used,(int)((sizeof(unsigned long)*CHAR_BIT+DIGIT_BIT-1)/DIGIT_BIT))-1;

      /* get most significant digit of result */
      res = DIGIT(a,i);

      while(--i >= 0) {
        res = (res << DIGIT_BIT) | DIGIT(a,i);
      }

      return res;
  }

  static void twos_complement MPA(mp_int* a) {
    int i = a->used;

    while(i--) {
      DIGIT(a,i) = (~DIGIT(a,i)) & (DIGIT_RADIX-1);
    }
    mp_sub_d(MPST, a, 1, a);
  }

#define BITWISE_OP_AND 1
#define BITWISE_OP_OR  2
#define BITWISE_OP_XOR 3

  static void bignum_bitwise_op MPA(int op, mp_int* x, mp_int* y, mp_int* n) {
    mp_int a, b;
    mp_int* d1;
    mp_int* d2;

    int i, sign, l1,  l2;
    char origin_sign_x = x->sign;
    char origin_sign_y = y->sign;

    mp_init(&a);
    mp_init(&b);

    if(y->sign == MP_NEG) {
      mp_copy(MPST, y, &b);
      twos_complement(MPST, &b);
      b.used = y->used;
      y = &b;
    }

    if(x->sign == MP_NEG) {
      mp_copy(MPST, x, &a);
      twos_complement(MPST, &a);
      a.used = x->used;
      x = &a;
    }

    if(x->used > y->used) {
      l1 = y->used;
      l2 = x->used;
      d1 = y;
      d2 = x;
      sign = y->sign;
    } else {
      l1 = x->used;
      l2 = y->used;
      d1 = x;
      d2 = y;
      sign = x->sign;
    }

    mp_grow(MPST, n, l2);
    n->used = l2;
    n->sign = MP_ZPOS;

    switch(op) {
      case BITWISE_OP_AND:
        if(origin_sign_x == MP_NEG && origin_sign_y == MP_NEG) n->sign = MP_NEG;

        for(i=0; i < l1; i++) {
          DIGIT(n,i) = DIGIT(d1,i) & DIGIT(d2,i);
        }

        for(; i < l2; i++) {
          DIGIT(n,i) = (sign == MP_ZPOS) ? 0 : DIGIT(d2,i);
        }

        break;
      case BITWISE_OP_OR:
        if(origin_sign_x == MP_NEG || origin_sign_y == MP_NEG) n->sign = MP_NEG;

        for(i=0; i < l1; i++) {
          DIGIT(n,i) = DIGIT(d1,i) | DIGIT(d2,i);
        }

        for(; i < l2; i++) {
          DIGIT(n,i) = (sign == MP_ZPOS) ? DIGIT(d2,i) : (DIGIT_RADIX-1);
        }

        break;
      case BITWISE_OP_XOR:
        if(origin_sign_x != origin_sign_y) n->sign = MP_NEG;

        for(i=0; i < l1; i++) {
          DIGIT(n,i) = DIGIT(d1,i) ^ DIGIT(d2,i);
        }

        for(; i < l2; i++) {
          DIGIT(n,i) = (sign == MP_ZPOS) ?
                          DIGIT(d2,i) :
                          (~DIGIT(d2,i) & (DIGIT_RADIX-1));
        }
        break;
    }

    if(n->sign == MP_NEG) {
      twos_complement(MPST, n);
    }

    /* free allocated resources for twos complement copies */
    mp_clear(&a);
    mp_clear(&b);
  }

  void Bignum::init(STATE) {
    GO(bignum).set(ontology::new_class(state, "Bignum", G(integer)));
    G(bignum)->set_object_type(state, BignumType);
  }

  namespace {
    // Cripped and modified from bn_mp_init.c
    void mp_init_managed(STATE, mp_int* a) {
      ByteArray* storage = ByteArray::create_dirty(state, sizeof (mp_digit) * MP_PREC);
      a->managed = reinterpret_cast<void*>(storage);

      /* allocate memory required and clear it */
      a->dp = OPT_CAST(mp_digit)storage->raw_bytes();

      /* set the digits to zero */
      for(int i = 0; i < MP_PREC; i++) {
        a->dp[i] = 0;
      }

      /* set the used to zero, allocated digits to the default precision
       * and sign to positive */
      a->used  = 0;
      a->alloc = MP_PREC;
      a->sign  = MP_ZPOS;
    }
  }

  Bignum* Bignum::create(STATE) {
    Bignum* o = state->new_object_dirty<Bignum>(G(bignum));
    mp_init_managed(state, o->mp_val());
    return o;
  }

  Bignum* Bignum::initialize_copy(STATE, Bignum* other) {
    mp_copy(XST, mp_val(), other->mp_val());
    return this;
  }

  Bignum* Bignum::from(STATE, int num) {
    Bignum* o = Bignum::create(state);
    mp_int* a = o->mp_val();

    if(num < 0) {
      // We can't invert num if it's the minimal value, since
      // that is not representable in a signed value. Therefore
      // we add one, invert it and fix it up.
      if(num == INT_MIN) {
        long tmp = num + 1;
        mp_set_int(XST, a, (unsigned int)-tmp);
        mp_add_d(XST, a, 1, a);
      } else {
        mp_set_int(XST, a, (unsigned int)-num);
      }
      a->sign = MP_NEG;
    } else {
      mp_set_int(XST, a, (unsigned int)num);
    }
    return o;
  }

  Bignum* Bignum::from(STATE, unsigned int num) {
    Bignum* o = Bignum::create(state);
    mp_set_int(XST, o->mp_val(), num);
    return o;
  }

  Bignum* Bignum::from(STATE, long num) {
    Bignum* o = Bignum::create(state);
    mp_int* a = o->mp_val();

    if(num < 0) {
      // We can't invert num if it's the minimal value, since
      // that is not representable in a signed value. Therefore
      // we add one, invert it and fix it up.
      if(num == LONG_MIN) {
        long tmp = num + 1;
        mp_set_long(XST, a, (unsigned long)-tmp);
        mp_add_d(XST, a, 1, a);
      } else {
        mp_set_long(XST, a, (unsigned long)-num);
      }
      a->sign = MP_NEG;
    } else {
      mp_set_long(XST, a, (unsigned long)num);
    }
    return o;
  }

  Bignum* Bignum::from(STATE, unsigned long num) {
    Bignum* o = Bignum::create(state);
    mp_set_long(XST, o->mp_val(), num);
    return o;
  }

  Bignum* Bignum::from(STATE, unsigned long long val) {
    Bignum* ret = Bignum::create(state);
    mp_int* num = ret->mp_val();

    mp_set_long(XST, num, val & 0xffffffff);

    mp_int high;
    mp_init_set_int(XST, &high, val >> 32);
    mp_mul_2d(XST, &high, 32, &high);

    mp_or(XST, num, &high, num);

    mp_clear(&high);

    return ret;
  }

  Bignum* Bignum::from(STATE, long long val) {
    Bignum* ret;

    if(val < 0) {
      // We can't invert num if it's the minimal value, since
      // that is not representable in a signed value. Therefore
      // we add one, invert it and fix it up.
      if(val == LLONG_MIN) {
        long long tmp = val + 1;
        ret = Bignum::from(state, (unsigned long long)-tmp);
        mp_add_d(XST, ret->mp_val(), 1, ret->mp_val());
      } else {
        ret = Bignum::from(state, (unsigned long long)-val);
      }
      ret->mp_val()->sign = MP_NEG;
    } else {
      ret = Bignum::from(state, (unsigned long long)val);
    }

    return ret;
  }

  Integer* Bignum::from(STATE, mp_int *num) {
    if((size_t)mp_count_bits(num) <= FIXNUM_WIDTH) {
      unsigned long val = mp_get_long(num);
      return num->sign == MP_NEG ? Fixnum::from(-val) : Fixnum::from(val);
    } else {
      Bignum* n_obj = Bignum::create(state);
      mp_copy(XST, num, n_obj->mp_val());
      return n_obj;
    }
  }

  Bignum* Bignum::create(STATE, Fixnum* val) {
    return Bignum::from(state, val->to_native());
  }

  native_int Bignum::to_native() {
    return (mp_val()->sign == MP_NEG) ? -mp_get_long(mp_val()) : mp_get_long(mp_val());
  }

  int Bignum::to_int() {
    return (mp_val()->sign == MP_NEG) ? -mp_get_int(mp_val()) : mp_get_int(mp_val());
  }

  unsigned int Bignum::to_uint() {
    return mp_get_int(mp_val());
  }

  long Bignum::to_long() {
    return (mp_val()->sign == MP_NEG) ? -mp_get_long(mp_val()) : mp_get_long(mp_val());
  }

  unsigned long Bignum::to_ulong() {
    return mp_get_long(mp_val());
  }

  long long Bignum::to_long_long() {
    mp_int *s = mp_val();
    return (s->sign == MP_NEG) ? -to_ulong_long() : to_ulong_long();
  }

  unsigned long long Bignum::to_ulong_long() {
    mp_int t;
    mp_int* s = mp_val();
    unsigned long long out, tmp;

    /* mp_get_int() gets only the lower 32 bits, on any platform. */
    out = mp_get_int(s);

    mp_init(&t);
    mp_div_2d(0, s, 32, &t, NULL);

    tmp = mp_get_int(&t);
    out |= tmp << 32;

    mp_clear(&t);
    return out;
  }

  bool Bignum::positive_p() {
    return mp_val()->sign != MP_NEG;
  }

  bool Bignum::even_p() {
    return mp_iseven(mp_val()) == MP_YES;
  }

  Integer* Bignum::normalize(STATE, Bignum* b) {
    mp_clamp(b->mp_val());

    if((size_t)mp_count_bits(b->mp_val()) <= FIXNUM_WIDTH) {
      native_int val;
      val = (native_int)b->to_native();
      return Fixnum::from(val);
    }
    return b;
  }

  Integer* Bignum::abs(STATE) {
    if(mp_val()->sign == MP_NEG) {
      return Bignum::from(state, 0)->sub(state, this);
    } else {
      return this;
    }
  }

  Integer* Bignum::add(STATE, Fixnum* b) {
    NMP;
    native_int bi = b->to_native();
    if(bi > 0) {
      mp_add_d(XST, mp_val(), bi, n);
    } else {
      mp_sub_d(XST, mp_val(), -bi, n);
    }
    return Bignum::normalize(state, n_obj);
  }

  Integer* Bignum::add(STATE, Bignum* b) {
    NMP;
    mp_add(XST, mp_val(), b->mp_val(), n);
    return Bignum::normalize(state, n_obj);
  }

  Float* Bignum::add(STATE, Float* b) {
    return b->add(state, this);
  }

  Integer* Bignum::sub(STATE, Fixnum* b) {
    NMP;
    native_int bi = b->to_native();
    if(bi > 0) {
      mp_sub_d(XST, mp_val(), bi, n);
    } else {
      mp_add_d(XST, mp_val(), -bi, n);
    }
    return Bignum::normalize(state, n_obj);
  }

  Integer* Bignum::sub(STATE, Bignum* b) {
    NMP;
    mp_sub(XST, mp_val(), b->mp_val(), n);
    return Bignum::normalize(state, n_obj);
  }

  Float* Bignum::sub(STATE, Float* b) {
    return Float::coerce(state, this)->sub(state, b);
  }

  Integer* Bignum::mul(STATE, Fixnum* b) {
    NMP;

    native_int bi = b->to_native();
    if(bi == 2) {
      mp_mul_2(XST, mp_val(), n);
    } else {
      if(bi > 0) {
        mp_mul_d(XST, mp_val(), bi, n);
      } else {
        mp_mul_d(XST, mp_val(), -bi, n);
        mp_neg(XST, n, n);
      }
    }
    return Bignum::normalize(state, n_obj);
  }

  Integer* Bignum::mul(STATE, Bignum* b) {
    NMP;
    mp_mul(XST, mp_val(), b->mp_val(), n);
    return Bignum::normalize(state, n_obj);
  }

  Float* Bignum::mul(STATE, Float* b) {
    return b->mul(state, this);
  }

  Integer* Bignum::divide(STATE, Fixnum* denominator, Integer** remainder) {
    if(denominator->to_native() == 0) {
      Exception::zero_division_error(state, "divided by 0");
    }

    NMP;

    native_int bi  = denominator->to_native();
    mp_digit r;
    if(bi < 0) {
      mp_div_d(XST, mp_val(), -bi, n, &r);
      mp_neg(XST, n, n);
    } else {
      mp_div_d(XST, mp_val(), bi, n, &r);
    }

    if(remainder) {
      if(mp_val()->sign == MP_NEG) {
        *remainder = Fixnum::from(-(native_int)r);
      } else {
        *remainder = Fixnum::from((native_int)r);
      }
    }

    if(r != 0 && mp_cmp_d(n, 0) == MP_LT) {
      if(remainder) {
        *remainder = Fixnum::from(as<Fixnum>(*remainder)->to_native() + bi);
      }
      mp_sub_d(XST, n, 1, n);
    }
    return Bignum::normalize(state, n_obj);
  }

  Integer* Bignum::divide(STATE, Bignum* b, Integer** remainder) {
    if(mp_iszero(b->mp_val())) {
      Exception::zero_division_error(state, "divided by 0");
    }

    NMP;
    MMP;

    mp_div(XST, mp_val(), b->mp_val(), n, m);
    if(mp_val()->sign != b->mp_val()->sign && !mp_iszero(m)) {
      mp_sub_d(XST, n, 1, n);
      mp_mul(XST, b->mp_val(), n, m);
      mp_sub(XST, mp_val(), m, m);
    }

    if(remainder) {
      *remainder = Bignum::normalize(state, m_obj);
    }
    return Bignum::normalize(state, n_obj);
  }

  Integer* Bignum::div(STATE, Fixnum* denominator) {
    return divide(state, denominator, NULL);
  }

  Integer* Bignum::div(STATE, Bignum* denominator) {
    return divide(state, denominator, NULL);
  }

  Array* Bignum::divmod(STATE, Fixnum* denominator) {
    Integer* mod = Fixnum::from(0);
    Integer* quotient = divide(state, denominator, &mod);

    Array* ary = Array::create(state, 2);
    ary->set(state, 0, quotient);
    ary->set(state, 1, mod);

    return ary;
  }

  Array* Bignum::divmod(STATE, Bignum* denominator) {
    Integer* mod = Fixnum::from(0);
    Integer* quotient = divide(state, denominator, &mod);

    Array* ary = Array::create(state, 2);
    ary->set(state, 0, quotient);
    ary->set(state, 1, mod);
    return ary;
  }

  Array* Bignum::divmod(STATE, Float* denominator) {
    return Float::coerce(state, this)->divmod(state, denominator);
  }

  Integer* Bignum::mod(STATE, Fixnum* denominator) {
    Integer* mod = Fixnum::from(0);
    divide(state, denominator, &mod);
    return mod;
  }

  Integer* Bignum::mod(STATE, Bignum* denominator) {
    Integer* mod = Fixnum::from(0);
    divide(state, denominator, &mod);
    return mod;
  }

  Float* Bignum::mod(STATE, Float* denominator) {
    return Float::coerce(state, this)->mod(state, denominator);
  }

  Integer* Bignum::bit_and(STATE, Integer* b) {
    NMP;

    if(kind_of<Fixnum>(b)) {
      b = Bignum::from(state, b->to_native());
    }

    /* Perhaps this should use mp_and rather than our own version */
    bignum_bitwise_op(XST, BITWISE_OP_AND, mp_val(), as<Bignum>(b)->mp_val(), n);
    return Bignum::normalize(state, n_obj);
  }

  Integer* Bignum::bit_and(STATE, Float* b) {
    if(!LANGUAGE_18_ENABLED) {
      Exception::type_error(state, "can't convert Float into Integer for bitwise arithmetic");
    }
    return bit_and(state, Bignum::from_double(state, b->val));
  }

  Integer* Bignum::bit_or(STATE, Integer* b) {
    NMP;

    if(kind_of<Fixnum>(b)) {
      b = Bignum::from(state, b->to_native());
    }

    /* Perhaps this should use mp_or rather than our own version */
    bignum_bitwise_op(XST, BITWISE_OP_OR, mp_val(), as<Bignum>(b)->mp_val(), n);
    return Bignum::normalize(state, n_obj);
  }

  Integer* Bignum::bit_or(STATE, Float* b) {
    if(!LANGUAGE_18_ENABLED) {
      Exception::type_error(state, "can't convert Float into Integer for bitwise arithmetic");
    }
    return bit_or(state, Bignum::from_double(state, b->val));
  }

  Integer* Bignum::bit_xor(STATE, Integer* b) {
    NMP;

    if(kind_of<Fixnum>(b)) {
      b = Bignum::from(state, b->to_native());
    }
    /* Perhaps this should use mp_xor rather than our own version */
    bignum_bitwise_op(XST, BITWISE_OP_XOR, mp_val(), as<Bignum>(b)->mp_val(), n);
    return Bignum::normalize(state, n_obj);
  }

  Integer* Bignum::bit_xor(STATE, Float* b) {
    if(!LANGUAGE_18_ENABLED) {
      Exception::type_error(state, "can't convert Float into Integer for bitwise arithmetic");
    }
    return bit_xor(state, Bignum::from_double(state, b->val));
  }

  Integer* Bignum::invert(STATE) {
    NMP;

    /* inversion by -(a)-1 */
    mp_neg(XST, mp_val(), n);
    mp_sub_d(XST, n, 1, n);

    return Bignum::normalize(state, n_obj);
  }

  Integer* Bignum::neg(STATE) {
    NMP;

    mp_neg(XST, mp_val(), n);
    return Bignum::normalize(state, n_obj);
  }

  /* These 2 don't use mp_lshd because it shifts by internal digits,
     not bits. */

  Integer* Bignum::left_shift(STATE, Fixnum* bits) {
    NMP;
    native_int shift = bits->to_native();
    if(shift < 0) {
      return right_shift(state, Fixnum::from(-bits->to_native()));
    }

    mp_int* a = mp_val();

    mp_mul_2d(XST, a, shift, n);
    n->sign = a->sign;
    return Bignum::normalize(state, n_obj);
  }

  Integer* Bignum::right_shift(STATE, Fixnum* bits) {
    NMP;
    native_int shift = bits->to_native();

    if(shift < 0) {
      return left_shift(state, Fixnum::from(-bits->to_native()));
    }

    mp_int* a = mp_val();

    if((shift / DIGIT_BIT) >= a->used) {
      if(a->sign == MP_ZPOS) {
        return Fixnum::from(0);
      } else {
        return Fixnum::from(-1);
      }
    }

    if(shift == 0) {
      mp_copy(XST, a, n);
    } else {
      mp_div_2d(XST, a, shift, n, NULL);

      // Because mp_int is not stored in twos complement format for
      // negative values, we have to simulate the effects of being
      // twos complement. Namely, if we shift past a set bit, we have
      // to subtract 1 because that show up as the shifted twos
      // complement representation.
      //
      // Observe:
      //
      // 10:55 MenTaLguY: 5 is 0101, -5 is 1011
      // 10:55 MenTaLguY: 0101 >> 1 = 0010, 1011 >> 1 = 1101
      // 10:56 MenTaLguY: 2 and -3 respectively
      //
      // and
      //
      // 10:57 MenTaLguY: 6 = 0110 and -6 = 1010
      //    0110 >> 1 == 0011, 1010 >> 1 == 1101
      //    3 and -3, respectively
      //

      if(a->sign == MP_NEG) {
        bool bit_inside_shift = false;
        int full_digits = shift / DIGIT_BIT;

        for(int d = 0; d < full_digits; d++) {
          if(DIGIT(a, d) != 0) {
            bit_inside_shift = true;
            break;
          }
        }

        if(!bit_inside_shift) {
          native_int shift_mask = ((native_int)1 << (shift % DIGIT_BIT)) - 1;
          bit_inside_shift = (DIGIT(a, full_digits) & shift_mask);
        }

        if(bit_inside_shift) {
          mp_sub_d(XST, n, 1, n);
        }
      }

    }

    return Bignum::normalize(state, n_obj);
  }

  Object* Bignum::pow(STATE, Fixnum *exponent) {
    NMP;

    native_int exp = exponent->to_native();

    if(!LANGUAGE_18_ENABLED && exp < 0) {
      return Primitives::failure();
    }

    if(exp < 0) {
      return this->to_float(state)->fpow(state, exponent);
    }

    mp_expt_d(XST, mp_val(), exp, n);

    return Bignum::normalize(state, n_obj);
  }

  Object* Bignum::pow(STATE, Bignum *exponent) {
    if(!LANGUAGE_18_ENABLED && CBOOL(exponent->lt(state, Fixnum::from(0)))) {
      return Primitives::failure();
    }

    return this->to_float(state)->fpow(state, exponent);
  }

  Object* Bignum::pow(STATE, Float *exponent) {
    return this->to_float(state)->fpow(state, exponent);
  }

  Object* Bignum::equal(STATE, Fixnum* b) {
    native_int bi = b->to_native();
    mp_int* a = mp_val();
    bool clear_a = false;
    mp_int n;

    if(bi < 0) {
      bi = -bi;
      mp_init_copy(XST, &n, a);
      mp_neg(XST, &n, &n);
      a = &n;
      clear_a = true;
    }

    int r = mp_cmp_d(a, bi);

    if(clear_a) {
      mp_clear(a);
    }

    if(r == MP_EQ) {
      return cTrue;
    }
    return cFalse;
  }

  Object* Bignum::equal(STATE, Bignum* b) {
    if(mp_cmp(mp_val(), b->mp_val()) == MP_EQ) {
      return cTrue;
    }
    return cFalse;
  }

  Object* Bignum::equal(STATE, Float* b) {
    return Float::coerce(state, this)->equal(state, b);
  }

  Object* Bignum::compare(STATE, Fixnum* b) {
    native_int bi = b->to_native();
    mp_int* a = mp_val();
    if(bi < 0) {
      mp_int n;
      mp_init_copy(XST, &n, a);
      mp_neg(XST, &n, &n);

      int r = mp_cmp_d(&n, -bi);

      mp_clear(&n);

      switch(r) {
        case MP_LT:
          return Fixnum::from(1);
        case MP_GT:
          return Fixnum::from(-1);
      }

    } else {
      switch(mp_cmp_d(a, bi)) {
        case MP_LT:
          return Fixnum::from(-1);
        case MP_GT:
          return Fixnum::from(1);
      }
    }
    return Fixnum::from(0);
  }

  Object* Bignum::compare(STATE, Bignum* b) {
    switch(mp_cmp(mp_val(), b->mp_val())) {
      case MP_LT:
        return Fixnum::from(-1);
      case MP_GT:
        return Fixnum::from(1);
    }
    return Fixnum::from(0);
  }

  Object* Bignum::compare(STATE, Float* b) {
    if(isinf(b->val)) {
      if(b->val > 0) {
        return Fixnum::from(-1);
      } else {
        return Fixnum::from(1);
      }
    }
    return Float::coerce(state, this)->compare(state, b);
  }

  Object* Bignum::gt(STATE, Fixnum* b) {
    native_int bi = b->to_native();

    mp_int* a = mp_val();
    if(bi < 0) {
      mp_int n;
      mp_init_copy(XST, &n, a);
      mp_neg(XST, &n, &n);

      int r = mp_cmp_d(&n, -bi);

      mp_clear(&n);

      if(r == MP_LT) {
        return cTrue;
      }
      return cFalse;
    } else {
      if(mp_cmp_d(a, bi) == MP_GT) {
        return cTrue;
      }
      return cFalse;
    }
  }

  Object* Bignum::gt(STATE, Bignum* b) {
    if(mp_cmp(mp_val(), b->mp_val()) == MP_GT) {
      return cTrue;
    }
    return cFalse;
  }

  Object* Bignum::gt(STATE, Float* b) {
    return Float::coerce(state, this)->gt(state, b);
  }

  Object* Bignum::ge(STATE, Fixnum* b) {
    native_int bi = b->to_native();

    mp_int* a = mp_val();
    if(bi < 0) {
      mp_int n;
      mp_init_copy(XST, &n, a);
      mp_neg(XST, &n, &n);
      int r = mp_cmp_d(&n, -bi);
      mp_clear(&n);
      if(r == MP_EQ || r == MP_LT) {
        return cTrue;
      }
      return cFalse;
    } else {
      int r = mp_cmp_d(a, bi);
      if(r == MP_EQ || r == MP_GT) {
        return cTrue;
      }
      return cFalse;
    }
  }

  Object* Bignum::ge(STATE, Float* b) {
    return Float::coerce(state, this)->ge(state, b);
  }

  Object* Bignum::ge(STATE, Bignum* b) {
    int r = mp_cmp(mp_val(), b->mp_val());
    if(r == MP_GT || r == MP_EQ) {
      return cTrue;
    }
    return cFalse;
  }

  Object* Bignum::lt(STATE, Fixnum* b) {
    native_int bi = b->to_native();

    mp_int* a = mp_val();
    if(bi < 0) {
      mp_int n;
      mp_init_copy(XST, &n, a);
      mp_neg(XST, &n, &n);

      int r = mp_cmp_d(&n, -bi);

      mp_clear(&n);

      if(r == MP_GT) {
        return cTrue;
      }
      return cFalse;
    } else {
      if(mp_cmp_d(a, bi) == MP_LT) {
        return cTrue;
      }
      return cFalse;
    }
  }

  Object* Bignum::lt(STATE, Bignum* b) {
    if(mp_cmp(mp_val(), b->mp_val()) == MP_LT) {
      return cTrue;
    }
    return cFalse;
  }

  Object* Bignum::lt(STATE, Float* b) {
    return Float::coerce(state, this)->lt(state, b);
  }

  Object* Bignum::le(STATE, Fixnum* b) {
    native_int bi = b->to_native();

    mp_int* a = mp_val();
    if(bi < 0) {
      mp_int n;
      mp_init_copy(XST, &n, a);
      mp_neg(XST, &n, &n);
      int r = mp_cmp_d(&n, -bi);
      mp_clear(&n);
      if(r == MP_EQ || r == MP_GT) {
        return cTrue;
      }
      return cFalse;
    } else {
      int r = mp_cmp_d(a, bi);
      if(r == MP_EQ || r == MP_LT) {
        return cTrue;
      }
      return cFalse;
    }
  }

  Object* Bignum::le(STATE, Bignum* b) {
    int r = mp_cmp(mp_val(), b->mp_val());
    if(r == MP_LT || r == MP_EQ) {
      return cTrue;
    }
    return cFalse;
  }

  Object* Bignum::le(STATE, Float* b) {
    return Float::coerce(state, this)->le(state, b);
  }

  Float* Bignum::to_float(STATE) {
    return Float::create(state, to_double(state));
  }

  String* Bignum::to_s(STATE, Fixnum* base) {
    native_int b = base->to_native();
    mp_int* self = mp_val();
    if(b < 2 || b > 36) {
      Exception::argument_error(state, "base must be between 2 and 36");
    }

    int sz = 0;
    int digits;
    mp_radix_size(self, b, &sz);
    if(sz == 0) {
      Exception::runtime_error(state, "couldn't convert bignum to string");
    }

    String* str = String::create(state, Fixnum::from(sz));
    mp_toradix_nd(XST, mp_val(), (char*)str->byte_address(), b, sz, &digits);
    if(self->sign == MP_NEG) { digits++; }
    str->num_bytes(state, Fixnum::from(digits));
    str->encoding(state, Encoding::usascii_encoding(state));

    return str;
  }

  Integer* Bignum::from_string_detect(STATE, const char *str) {
    const char *s;
    int radix;
    int sign;
    mp_int n;
    mp_init(&n);

    s = str;
    sign = 1;

    while(isspace(*s)) { s++; }

    if(*s == '+') {
      s++;
    } else if(*s == '-') {
      sign = 0;
      s++;
    }

    radix = 10;
    if(*s == '0') {
      switch(s[1]) {
      case 'x': case 'X':
        radix = 16; s += 2;
        break;
      case 'b': case 'B':
        radix = 2; s += 2;
        break;
      case 'o': case 'O':
        radix = 8; s += 2;
        break;
      case 'd': case 'D':
        radix = 10; s += 2;
        break;
      default:
        radix = 8; s += 1;
      }
    }

    mp_read_radix(XST, &n, s, radix);

    if(!sign) {
      n.sign = MP_NEG;
    }

    Integer* res = Bignum::from(state, &n);

    mp_clear(&n);

    return res;
  }

  Integer* Bignum::from_string(STATE, const char *str, size_t radix) {
    char *endptr = NULL;
    long l;

    errno = 0;
    l = strtol(str, &endptr, radix);

    if(endptr != str && errno == 0 &&
       l >= FIXNUM_MIN && l <= FIXNUM_MAX) {
      return Fixnum::from(l);
    }

    mp_int n;
    mp_init(&n);
    mp_read_radix(XST, &n, str, radix);

    Integer* res = Bignum::from(state, &n);

    mp_clear(&n);

    return res;
  }

  void Bignum::into_string(STATE, size_t radix, char *buf, size_t sz) {
    int k;
    mp_toradix_nd(XST, mp_val(), buf, radix, sz, &k);
  }

  Integer* Bignum::from_array(STATE, uint32_t *ary, size_t sz) {
    /*
     * Read the values from the given array to populate a bignum.
     *
     * ary[0] contains the least significant sizeof(ary[0]) * 8 bits;
     * ary[1] the next least significant, etc.
     *
     * See also Bignum::into_array()
     */

    Bignum* big = Bignum::create(state);

    for(int i = sz - 1; i >= 0; i--) {
      Integer* tmp = big->left_shift(state, Fixnum::from(32));
      big = tmp->fixnum_p() ?
              Bignum::from(state, tmp->to_native()) :
              as<Bignum>(tmp);

      tmp = big->bit_or(state, Bignum::from(state, ary[i]));

      big = tmp->fixnum_p() ?
              Bignum::from(state, tmp->to_native()) :
              as<Bignum>(tmp);
    }

    return Bignum::normalize(state, big);
  }

  size_t Bignum::into_array(STATE, uint32_t *ary, size_t sz) {
    /*
     * Split the bignum's value into an array of unsigned integers.
     *
     * After execution, ary[0] contains the least significant
     * sizeof(ary[0]) * 8 bits; ary[1] the next least significant, etc.
     *
     * See also Bignum::from_array()
     */

    if(ary) ary[0] = 0;

    uint32_t n = 0;
    Integer* rest_i = this->abs(state);

    Bignum* rest = rest_i->fixnum_p() ?
                      Bignum::from(state, rest_i->to_native()) :
                      as<Bignum>(rest_i);

    for(;;) {
      if(ary && n < sz) {
        ary[n] = rest->to_ulong() & 0xffffffff;
      }

      n++;

      rest_i = rest->right_shift(state, Fixnum::from(32));

      if(rest_i->fixnum_p()) {
        native_int rest_n = rest_i->to_native();
        if(rest_n == 0) break;

        rest = Bignum::from(state, rest_n);
      } else {
        rest = as<Bignum>(rest_i);
      }
    }

    return n;
  }

  double Bignum::to_double(STATE) {
    mp_int* a = mp_val();

    /* get number of digits of the lsb we have to read */
    int i = a->used;

    double res = 0.0;

    /* loop down from the most significant digit of result */
    while(i > 0) {
      --i;
      res = (res * (double)DIGIT_RADIX) + DIGIT(a,i);
    }

    if(isinf(res)) {
      /* Bignum out of range */
      res = HUGE_VAL;
    }

    if(a->sign == MP_NEG) res = -res;

    return res;
  }

  Integer* Bignum::from_float(STATE, Float* f) {
    return Bignum::from_double(state, f->val);
  }

  Integer* Bignum::from_double(STATE, double d) {
    NMP;

    long i = 0;
    double value;

    value = (d < 0) ? -d : d;

    if(isinf(d)) {
      Exception::float_domain_error(state, d < 0 ? "-Infinity" : "Infinity");
    } else if(isnan(d)) {
      Exception::float_domain_error(state, "NaN");
    }

    while(!(value <= (LONG_MAX >> 1)) || 0 != (long)value) {
      value = value / (double)(DIGIT_RADIX);
      i++;
    }

    mp_grow(XST, n, i);

    while(i--) {
      value *= DIGIT_RADIX;
      BDIGIT_DBL c = (BDIGIT_DBL) value;
      value -= c;
      DIGIT(n,i) = c;
      n->used += 1;
    }

    if(d < 0) {
      mp_neg(XST, n, n);
    }

    return Bignum::normalize(state, n_obj);
  }

  Array* Bignum::coerce(STATE, Bignum* other) {
    Array* ary = Array::create(state, 2);

    ary->set(state, 0, other);
    ary->set(state, 1, this);

    return ary;
  }

  Array* Bignum::coerce(STATE, Fixnum* other) {
    Array* ary = Array::create(state, 2);

    if(Fixnum* fix = try_as<Fixnum>(Bignum::normalize(state, this))) {
      ary->set(state, 0, other);
      ary->set(state, 1, fix);
    } else {
      ary->set(state, 0, Bignum::create(state, other));
      ary->set(state, 1, this);
    }

    return ary;
  }

  Integer* Bignum::size(STATE) {
    int bits = mp_count_bits(mp_val());
    int bytes = (bits + 7) / 8;

    /* MRI returns this in words, but thats an implementation detail as far
       as I'm concerned. */
    return Fixnum::from(bytes);
  }

  void Bignum::verify_size(STATE, size_t size) {
    size_t bits = mp_count_bits(mp_val());

    if(bits > size) {
      std::ostringstream msg;
      msg << "Bignum too large to fit in " << size << " bits";
      Exception::range_error(state, msg.str().c_str());
    }
  }

  hashval Bignum::hash_bignum(STATE) {
    mp_int* a = mp_val();

    /* Apparently, a couple bits of each a->dp[n] aren't actually used,
       (e.g. when DIGIT_BIT is 60) so this hash is actually including
       that unused memory.  This might only be a problem if calculations
       are leaving cruft in those unused bits.  However, since Bignums
       are immutable, this shouldn't happen to us. */
    return String::hash_str(state, (unsigned char *)a->dp, a->used * sizeof(mp_digit));
  }

  size_t Bignum::managed_memory_size(STATE) {
    mp_int* n = this->mp_val();
    assert(MANAGED(n));
    Object* m = static_cast<Object*>(n->managed);
    return m->size_in_bytes(state->vm());
  }

  extern "C" void* MANAGED_REALLOC_MPINT(void* s, mp_int* a, size_t bytes) {
    assert(s);
    State* state = reinterpret_cast<State*>(s);

    ByteArray* storage = ByteArray::create_dirty(state, bytes);
    a->managed = reinterpret_cast<void*>(storage);

    // Be sure to use the smaller value!
    size_t current_bytes = sizeof(mp_digit) * (size_t)a->alloc;
    size_t to_copy = bytes;

    if(current_bytes < bytes) to_copy = current_bytes;

    memcpy(storage->raw_bytes(), a->dp, to_copy);

    return storage->raw_bytes();
  }

  void Bignum::Info::mark(Object* obj, ObjectMark& mark) {
    Bignum* big = force_as<Bignum>(obj);

    mp_int* n = big->mp_val();
    assert(MANAGED(n));

    Object* tmp = mark.call(static_cast<Object*>(n->managed));
    if(tmp && tmp != n->managed) {
      n->managed = reinterpret_cast<void*>(tmp);
      ByteArray* ba = force_as<ByteArray>(tmp);
      n->dp = OPT_CAST(mp_digit)ba->raw_bytes();
    }
  }

  void Bignum::Info::show(STATE, Object* self, int level) {
    Bignum* b = as<Bignum>(self);
    std::cout << b->to_s(state, Fixnum::from(10))->c_str(state) << std::endl;
  }

  void Bignum::Info::show_simple(STATE, Object* self, int level) {
    show(state, self, level);
  }


}

