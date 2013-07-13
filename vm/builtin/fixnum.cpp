#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/encoding.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/string.hpp"
#include "configuration.hpp"
#include "object_utils.hpp"
#include "ontology.hpp"
#include "version.h"

namespace rubinius {

  void Fixnum::init(STATE) {
    GO(fixnum_class).set(ontology::new_class(state, "Fixnum", G(integer)));
    G(fixnum_class)->set_object_type(state, FixnumType);

    G(fixnum_class)->set_const(state, "MIN", Fixnum::from(FIXNUM_MIN));
    G(fixnum_class)->set_const(state, "MAX", Fixnum::from(FIXNUM_MAX));
  }

  int Fixnum::to_int() const {
    return (int)STRIP_FIXNUM_TAG(this);
  }

  unsigned int Fixnum::to_uint() const {
    return (unsigned int)STRIP_FIXNUM_TAG(this);
  }

  long Fixnum::to_long() const {
    return (long)STRIP_FIXNUM_TAG(this);
  }

  unsigned long Fixnum::to_ulong() const {
    return (unsigned long)STRIP_FIXNUM_TAG(this);
  }

  long long Fixnum::to_long_long() const {
    return (long long)STRIP_FIXNUM_TAG(this);
  }

  unsigned long long Fixnum::to_ulong_long() const {
    return (unsigned long long)STRIP_FIXNUM_TAG(this);
  }

  bool Fixnum::positive_p() const {
    return to_native() >= 0;
  }

  Integer* Fixnum::add(STATE, Bignum* other) {
    return other->add(state, this);
  }

  Float* Fixnum::add(STATE, Float* other) {
    return other->add(state, this);
  }

  Integer* Fixnum::sub(STATE, Bignum* other) {
    return as<Bignum>(other->neg(state))->add(state, this);
  }

  Float* Fixnum::sub(STATE, Float* other) {
    return Float::coerce(state, this)->sub(state, other);
  }

#define SQRT_LONG_MAX ((native_int)1<<((sizeof(native_int)*CHAR_BIT-1)/2))
/*tests if N*N would overflow*/
#define FIT_SQRT(n) (((n)<SQRT_LONG_MAX)&&((n)>-SQRT_LONG_MAX))
// Adapted from the logic in 1.9
#define NO_OVERFLOW_MUL(a,b) (FIT_SQRT(a)&&FIT_SQRT(b))
#define OVERFLOW_MUL(a,b) (!(NO_OVERFLOW_MUL(a,b)))

  Integer* Fixnum::mul(STATE, Fixnum* other) {
    native_int a  = to_native();
    native_int b  = other->to_native();

    if(a == 0 || b == 0) return Fixnum::from(0);

    if(NO_OVERFLOW_MUL(a, b)) {
      return Fixnum::from(a * b);
    }

    return Bignum::from(state, a)->mul(state, other);
  }

  Integer* Fixnum::mul(STATE, Bignum* other) {
    return other->mul(state, this);
  }

  Float* Fixnum::mul(STATE, Float* other) {
    return other->mul(state, this);
  }

  Integer* Fixnum::div(STATE, Fixnum* other) {
    if(other->to_native() == 0) {
      Exception::zero_division_error(state, "divided by 0");
    }
    native_int numerator = to_native();
    native_int denominator = other->to_native();
    native_int quotient;

    if(denominator < 0) {
      if(numerator < 0) {
        quotient = -numerator / -denominator;
      } else {
        quotient = - (numerator / -denominator);
      }
    } else {
      if(numerator < 0) {
        quotient = - (-numerator / denominator);
      } else {
        quotient = numerator / denominator;
      }
    }

    native_int mod = numerator - (quotient * denominator);
    if((mod < 0 && denominator > 0) || (mod > 0 && denominator < 0)) {
      quotient--;
    }

    return Fixnum::from(quotient);
  }

  Integer* Fixnum::div(STATE, Bignum* other) {
    return Bignum::from(state, to_native())->div(state, other);
  }

  Float* Fixnum::div(STATE, Float* other) {
    return Float::create(state, to_native())->div(state, other);
  }

  Integer* Fixnum::mod(STATE, Fixnum* other) {
    native_int numerator = to_native();
    native_int denominator = other->to_native();
    native_int quotient = div(state, other)->to_native();
    native_int modulo = numerator - denominator * quotient;

    if((modulo < 0 && denominator > 0) || (modulo > 0 && denominator < 0)) {
      return Fixnum::from(modulo + denominator);
    } else {
      return Fixnum::from(modulo);
    }
  }

  Integer* Fixnum::mod(STATE, Bignum* other) {
    return Bignum::from(state, to_native())->mod(state, other);
  }

  Float* Fixnum::mod(STATE, Float* other) {
    return Float::create(state, to_native())->mod(state, other);
  }

  Array* Fixnum::divmod(STATE, Fixnum* other) {
    if(other->to_native() == 0) {
      Exception::zero_division_error(state, "divided by 0");
    }
    native_int numerator = to_native();
    native_int denominator = other->to_native();
    native_int fraction = div(state, other)->to_native();
    Array* ary = Array::create(state, 2);
    ary->set(state, 0, Fixnum::from(fraction));
    ary->set(state, 1, Fixnum::from(numerator - denominator * fraction));
    return ary;
  }

  Array* Fixnum::divmod(STATE, Bignum* other) {
    return Bignum::from(state, to_native())->divmod(state, other);
  }

  Array* Fixnum::divmod(STATE, Float* other) {
    return Float::create(state, to_native())->divmod(state, other);
  }

  Integer* Fixnum::neg(STATE) {
    return Fixnum::from(-to_native());
  }

  Object* Fixnum::pow(STATE, Fixnum* exponent) {
    native_int base = to_native();
    native_int exp = exponent->to_native();

    if(!LANGUAGE_18_ENABLED && exp < 0) {
      return Primitives::failure();
    }

    if(exp == 0) return Fixnum::from(1);
    if(base == 1) return this;

    if(base == 0) {
      if(exp > 0) return Fixnum::from(0);
      return Float::create(state, INFINITY);
    }

    if(exp < 0) {
      return this->to_f(state)->fpow(state, exponent);
    }

    native_int result = 1;

    /*
     * Exponentiation by squaring algorithm
     * Based on the notion that x ** n == (x ** (n / 2)) ** 2
     * for even n's and x ** n == x * ((n - 1) / 2) ** 2 for
     * odd n's.
     */
    while(exp > 0) {
      if(exp & 1) {
        if(OVERFLOW_MUL(result, base)) {
          return Bignum::from(state, to_native())->pow(state, exponent);
        }
        result *= base;
      }
      // The exp > 1 check is to not overflow unnecessary if this is the
      // last iteration of the algorithm
      if(exp > 1) {
        if(!FIT_SQRT(base)) {
          return Bignum::from(state, to_native())->pow(state, exponent);
        }
        base *= base;
      }
      exp >>= 1;
    }
    return Fixnum::from(result);
  }

  Object* Fixnum::pow(STATE, Bignum* exponent) {
    if(!LANGUAGE_18_ENABLED && CBOOL(exponent->lt(state, Fixnum::from(0)))) {
      return Primitives::failure();
    }

    native_int i = to_native();
    if(i == 0 || i == 1) return this;
    if(i == -1) return Fixnum::from(exponent->even_p() ? 1 : -1);
    return Bignum::from(state, to_native())->pow(state, exponent);
  }

  Object* Fixnum::pow(STATE, Float* exponent) {
    return this->to_f(state)->fpow(state, exponent);
  }

  Object* Fixnum::equal(STATE, Fixnum* other) {
    return RBOOL(to_native() == other->to_native());
  }

  Object* Fixnum::equal(STATE, Bignum* other) {
    return other->equal(state, this);
  }

  Object* Fixnum::equal(STATE, Float* other) {
    return RBOOL((double)to_native() == other->val);
  }

  Object* Fixnum::compare(STATE, Fixnum* other) {
    native_int left  = to_native();
    native_int right = other->to_native();
    if(left == right) {
      return Fixnum::from(0);
    } else if(left < right) {
      return Fixnum::from(-1);
    } else {
      return Fixnum::from(1);
    }
  }

  Object* Fixnum::compare(STATE, Bignum* other) {
    native_int res = as<Fixnum>(other->compare(state, this))->to_native();
    if(res == 0) {
      return Fixnum::from(0);
    } else if(res < 0) {
      return Fixnum::from(1);
    } else {
      return Fixnum::from(-1);
    }
  }

  Object* Fixnum::compare(STATE, Float* other) {
    double left  = (double)to_native();
    double right = other->val;
    if(left == right) {
      return Fixnum::from(0);
    } else if(left < right) {
      return Fixnum::from(-1);
    } else {
      return Fixnum::from(1);
    }
  }

  Object* Fixnum::gt(STATE, Bignum* other) {
    return other->lt(state, this);
  }

  Object* Fixnum::gt(STATE, Float* other) {
    return RBOOL((double) to_native() > other->val);
  }

  Object* Fixnum::ge(STATE, Fixnum* other) {
    return RBOOL(to_native() >= other->to_native());
  }

  Object* Fixnum::ge(STATE, Bignum* other) {
    return other->le(state, this);
  }

  Object* Fixnum::ge(STATE, Float* other) {
    return RBOOL((double) to_native() >= other->val);
  }

  Object* Fixnum::lt(STATE, Bignum* other) {
    return other->gt(state, this);
  }

  Object* Fixnum::lt(STATE, Float* other) {
    return RBOOL((double) to_native() < other->val);
  }

  Object* Fixnum::le(STATE, Fixnum* other) {
    return RBOOL(to_native() <= other->to_native());
  }

  Object* Fixnum::le(STATE, Bignum* other) {
    return other->ge(state, this);
  }

  Object* Fixnum::le(STATE, Float* other) {
    return RBOOL((double) to_native() <= other->val);
  }

  Integer* Fixnum::left_shift(STATE, Fixnum* bits) {
    native_int shift = bits->to_native();
    if(shift < 0) {
      return right_shift(state, Fixnum::from(-shift));
    }

    native_int self = to_native();
    if(shift >= (native_int)FIXNUM_WIDTH) {
      return Bignum::from(state, self)->left_shift(state, bits);
    }

    native_int answer = self << shift;
    native_int check = answer >> shift;

    if(self != check) {
      return Bignum::from(state, self)->left_shift(state, bits);
    }

    if(answer > FIXNUM_MAX || answer < FIXNUM_MIN) {
      return Bignum::from(state, answer);
    } else {
      return Fixnum::from(answer);
    }
  }

  Integer* Fixnum::right_shift(STATE, Fixnum* bits) {
    native_int shift = bits->to_native();
    if(shift < 0) {
      return left_shift(state, Fixnum::from(-shift));
    }

    // boundary case. Don't overflow the bits back to their original
    // value like C does, just say it's 0.
    if(shift > (native_int)FIXNUM_WIDTH) {
      if(to_native() >= 0) return Fixnum::from(0);
      return Fixnum::from(-1);
    }

    return Fixnum::from(to_native() >> shift);
  }

  Integer* Fixnum::size(STATE) {
    return Fixnum::from(sizeof(native_int));
  }

  Integer* Fixnum::bit_and(STATE, Fixnum* other) {
    return Fixnum::from(to_native() & other->to_native());
  }

  Integer* Fixnum::bit_and(STATE, Bignum* other) {
    return other->bit_and(state, this);
  }

  Integer* Fixnum::bit_and(STATE, Float* other) {
    if(!LANGUAGE_18_ENABLED) {
      Exception::type_error(state, "can't convert Float into Integer for bitwise arithmetic");
    }
    return Fixnum::from(to_native() & (native_int)other->val);
  }

  Integer* Fixnum::bit_or(STATE, Fixnum* other) {
    return Fixnum::from(to_native() | other->to_native());
  }

  Integer* Fixnum::bit_or(STATE, Bignum* other) {
    return other->bit_or(state, this);
  }

  Integer* Fixnum::bit_or(STATE, Float* other) {
    if(!LANGUAGE_18_ENABLED) {
      Exception::type_error(state, "can't convert Float into Integer for bitwise arithmetic");
    }
    return Fixnum::from(to_native() | (native_int)other->val);
  }

  Integer* Fixnum::bit_xor(STATE, Fixnum* other) {
    return Fixnum::from(to_native() ^ other->to_native());
  }

  Integer* Fixnum::bit_xor(STATE, Bignum* other) {
    return other->bit_xor(state, this);
  }

  Integer* Fixnum::bit_xor(STATE, Float* other) {
    if(!LANGUAGE_18_ENABLED) {
      Exception::type_error(state, "can't convert Float into Integer for bitwise arithmetic");
    }
    return Fixnum::from(to_native() ^ (native_int)other->val);
  }

  Integer* Fixnum::invert(STATE) {
    return Fixnum::from(~to_native());
  }

  Float* Fixnum::to_f(STATE) {
    return Float::create(state, (double)to_native());
  }

  String* Fixnum::to_s(STATE) {
    return to_s(state, Fixnum::from(10));
  }

  static const char digitmap[] = "0123456789abcdefghijklmnopqrstuvwxyz";

  String* Fixnum::to_s(STATE, Fixnum* base) {
    // Base 2 fixnum with a minus sign and null byte is the maximum length
    char buf[FIXNUM_WIDTH + 2];
    char *b = buf + sizeof(buf);
    native_int j, k, m;

    j = base->to_native();
    k = to_native();

    if(j < 2 || j > 36) {
      Exception::argument_error(state, "base must be between 2 and 36");
    }

    /* Algorithm taken from 1.8.4 rb_fix2str */
    if(k == 0) return String::create(state, "0", 1);

    m = 0;
    if(k < 0) {
      k = -k;
      m = 1;
    }
    *--b = 0;
    do {
      *--b = digitmap[k % j];
    } while(k /= j);

    if(m) {
      *--b = '-';
    }

    String* str = String::create(state, b, buf + sizeof(buf) - b - 1);
    str->encoding(state, Encoding::usascii_encoding(state));

    return str;
  }

  Array* Fixnum::coerce(STATE, Bignum* other) {
    Array* ary = Array::create(state, 2);

    if(Fixnum* fix = try_as<Fixnum>(Bignum::normalize(state, other))) {
      ary->set(state, 0, fix);
      ary->set(state, 1, this);
    } else {
      ary->set(state, 0, other);
      ary->set(state, 1, Bignum::create(state, this));
    }

    return ary;
  }

  Array* Fixnum::coerce(STATE, Fixnum* other) {
    Array* ary = Array::create(state, 2);

    ary->set(state, 0, other);
    ary->set(state, 1, this);

    return ary;
  }

  void Fixnum::Info::show(STATE, Object* self, int level) {
    Fixnum* f = as<Fixnum>(self);
    std::cout << f->to_native() << std::endl;
  }

  void Fixnum::Info::show_simple(STATE, Object* self, int level) {
    show(state, self, level);
  }

  void Fixnum::Info::mark(Object* t, ObjectMark& mark) { }
}
