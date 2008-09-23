
#include "prelude.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/array.hpp"
#include "builtin/string.hpp"

#include "primitives.hpp"

#include <iostream>

namespace rubinius {
  /* WARNING. Do not use this version if +num+ has the chance of being
   * greater than FIXNUM_MAX or less than FIXNUM_MIN. */
  FIXNUM Fixnum::from(native_int num) {
    return (FIXNUM)APPLY_TAG(num, TAG_FIXNUM);
  }

  native_int Fixnum::to_native() const {
    return STRIP_TAG(this);
  }

  int Fixnum::to_int() const {
    return (int)STRIP_TAG(this);
  }

  unsigned int Fixnum::to_uint() const {
    return (unsigned int)STRIP_TAG(this);
  }

  long Fixnum::to_long() const {
    return (long)STRIP_TAG(this);
  }

  unsigned long Fixnum::to_ulong() const {
    return (unsigned long)STRIP_TAG(this);
  }

  long long Fixnum::to_long_long() const {
    return (long long)STRIP_TAG(this);
  }

  unsigned long long Fixnum::to_ulong_long() const {
    return (unsigned long long)STRIP_TAG(this);
  }

  INTEGER Fixnum::add(STATE, FIXNUM other) {
    native_int r = to_native() + other->to_native();
    if(r > FIXNUM_MAX || r < FIXNUM_MIN) {
      return Bignum::from(state, r);
    } else {
      return Fixnum::from(r);
    }
  }

  INTEGER Fixnum::add(STATE, Bignum* other) {
    return other->add(state, this);
  }

  Float* Fixnum::add(STATE, Float* other) {
    return other->add(state, this);
  }

  INTEGER Fixnum::sub(STATE, FIXNUM other) {
    native_int r = to_native() - other->to_native();
    if(r > FIXNUM_MAX || r < FIXNUM_MIN) {
      return Bignum::from(state, r);
    } else {
      return Fixnum::from(r);
    }
  }

  INTEGER Fixnum::sub(STATE, Bignum* other) {
    return as<Bignum>(other->neg(state))->add(state, this);
  }

  Float* Fixnum::sub(STATE, Float* other) {
    return Float::coerce(state, this)->sub(state, other);
  }

  INTEGER Fixnum::mul(STATE, FIXNUM other) {
    native_int a  = to_native();
    native_int b  = other->to_native();

    if(a == 0 || b == 0) return Fixnum::from(0);

    if(a > 0) {
      if(b > 0) {
        if(a > (FIXNUM_MAX / b)) {
          return Bignum::from(state, a)->mul(state, other);
        }
      } else {
        if (b < (FIXNUM_MIN / a)) {
          return Bignum::from(state, a)->mul(state, other);
        }
      }
    } else {
      if(b > 0){
        if(a < (FIXNUM_MIN / b)) {
          return Bignum::from(state, a)->mul(state, other);
        }
      } else {
        if(b < (FIXNUM_MAX / a)) {
          return Bignum::from(state, a)->mul(state, other);
        }
      }
    }

    return Fixnum::from(to_native() * other->to_native());
  }

  INTEGER Fixnum::mul(STATE, Bignum* other) {
    return other->mul(state, this);
  }

  Float* Fixnum::mul(STATE, Float* other) {
    return other->mul(state, this);
  }

  INTEGER Fixnum::div(STATE, FIXNUM other) {
    if(other->to_native() == 0) {
      throw ZeroDivisionError(other, "divided by 0");
    }
    native_int numerator = to_native();
    native_int denominator = other->to_native();
    native_int quotient = numerator / denominator;
    if(quotient < 0 && quotient * denominator != numerator) --quotient;
    return Fixnum::from(quotient);
  }

  INTEGER Fixnum::div(STATE, Bignum* other) {
    return Bignum::from(state, to_native())->div(state, other);
  }

  Float* Fixnum::div(STATE, Float* other) {
    return Float::coerce(state, this)->div(state, other);
  }

  INTEGER Fixnum::mod(STATE, FIXNUM other) {
    native_int numerator = to_native();
    native_int denominator = other->to_native();
    native_int quotient = div(state, other)->to_native();
    return Fixnum::from(numerator - denominator * quotient);
  }

  INTEGER Fixnum::mod(STATE, Bignum* other) {
    return Bignum::from(state, to_native())->mod(state, other);
  }

  Float* Fixnum::mod(STATE, Float* other) {
    return Float::create(state, to_native())->mod(state, other);
  }

  Array* Fixnum::divmod(STATE, FIXNUM other) {
    if(other->to_native() == 0) {
      throw ZeroDivisionError(other, "divided by 0");
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

  INTEGER Fixnum::neg(STATE) {
    return Fixnum::from(-to_native());
  }

  OBJECT Fixnum::equal(STATE, FIXNUM other) {
    return to_native() == other->to_native() ? Qtrue : Qfalse;
  }

  OBJECT Fixnum::equal(STATE, Bignum* other) {
    return other->equal(state, this);
  }

  OBJECT Fixnum::equal(STATE, Float* other) {
    return (double)to_native() == other->val ? Qtrue : Qfalse;
  }

  FIXNUM Fixnum::compare(STATE, FIXNUM other) {
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

  FIXNUM Fixnum::compare(STATE, Bignum* other) {
    native_int res = other->compare(state, this)->to_native();
    if(res == 0) {
      return Fixnum::from(0);
    } else if(res < 0) {
      return Fixnum::from(1);
    } else {
      return Fixnum::from(-1);
    }
  }

  FIXNUM Fixnum::compare(STATE, Float* other) {
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

  OBJECT Fixnum::gt(STATE, FIXNUM other) {
    return to_native() > other->to_native() ? Qtrue : Qfalse;
  }

  OBJECT Fixnum::gt(STATE, Bignum* other) {
    return other->lt(state, this);
  }

  OBJECT Fixnum::gt(STATE, Float* other) {
    return (double) to_native() > other->val ? Qtrue : Qfalse;
  }

  OBJECT Fixnum::ge(STATE, FIXNUM other) {
    return to_native() >= other->to_native() ? Qtrue : Qfalse;
  }

  OBJECT Fixnum::ge(STATE, Bignum* other) {
    return other->le(state, this);
  }

  OBJECT Fixnum::ge(STATE, Float* other) {
    return (double) to_native() >= other->val ? Qtrue : Qfalse;
  }

  OBJECT Fixnum::lt(STATE, FIXNUM other) {
    return to_native() < other->to_native() ? Qtrue : Qfalse;
  }

  OBJECT Fixnum::lt(STATE, Bignum* other) {
    return other->gt(state, this);
  }

  OBJECT Fixnum::lt(STATE, Float* other) {
    return (double) to_native() < other->val ? Qtrue : Qfalse;
  }

  OBJECT Fixnum::le(STATE, FIXNUM other) {
    return to_native() <= other->to_native() ? Qtrue : Qfalse;
  }

  OBJECT Fixnum::le(STATE, Bignum* other) {
    return other->ge(state, this);
  }

  OBJECT Fixnum::le(STATE, Float* other) {
    return (double) to_native() <= other->val ? Qtrue : Qfalse;
  }

  INTEGER Fixnum::left_shift(STATE, INTEGER bits) {
    native_int shift = bits->to_native();
    if(shift < 0) {
      return right_shift(state, Fixnum::from(-shift));
    }

    native_int self = to_native();

    if(shift > (native_int)FIXNUM_WIDTH || self >> ((native_int)FIXNUM_WIDTH - shift) > 0) {
      return Bignum::from(state, self)->left_shift(state, bits);
    }

    return Fixnum::from(self << shift);
  }

  INTEGER Fixnum::right_shift(STATE, INTEGER bits) {
    native_int shift = bits->to_native();
    if(shift < 0) {
      return left_shift(state, Fixnum::from(-shift));
    }

    native_int self = to_native();

    native_int a = self >> shift;
    return Fixnum::from(a);
  }

  INTEGER Fixnum::size(STATE) {
    return Fixnum::from(sizeof(native_int));
  }

  INTEGER Fixnum::bit_and(STATE, FIXNUM other) {
    return Fixnum::from(to_native() & other->to_native());
  }

  INTEGER Fixnum::bit_and(STATE, Bignum* other) {
    return other->bit_and(state, this);
  }

  INTEGER Fixnum::bit_and(STATE, Float* other) {
    return Fixnum::from(to_native() & (native_int)other->val);
  }

  INTEGER Fixnum::bit_or(STATE, FIXNUM other) {
    return Fixnum::from(to_native() | other->to_native());
  }

  INTEGER Fixnum::bit_or(STATE, Bignum* other) {
    return other->bit_or(state, this);
  }

  INTEGER Fixnum::bit_or(STATE, Float* other) {
    return Fixnum::from(to_native() | (native_int)other->val);
  }

  INTEGER Fixnum::bit_xor(STATE, FIXNUM other) {
    return Fixnum::from(to_native() ^ other->to_native());
  }

  INTEGER Fixnum::bit_xor(STATE, Bignum* other) {
    return other->bit_xor(state, this);
  }

  INTEGER Fixnum::bit_xor(STATE, Float* other) {
    return Fixnum::from(to_native() ^ (native_int)other->val);
  }

  INTEGER Fixnum::invert(STATE) {
    return Fixnum::from(~to_native());
  }

  Float* Fixnum::to_f(STATE) {
    return Float::create(state, (double)to_native());
  }

  String* Fixnum::to_s(STATE) {
    return to_s(state, Fixnum::from(10));
  }

  String* Fixnum::to_s(STATE, Fixnum* base) {
    // algorithm adapted from shotgun
    static const char digitmap[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char buf[100];
    char *b = buf + sizeof(buf);
    native_int j, k, m;

    j = base->to_native();
    k = to_native();

    if(j < 2 || j > 36) throw PrimitiveFailed();

    /* Algorithm taken from 1.8.4 rb_fix2str */
    if(k == 0) return String::create(state, "0");

    m = 0;
    if(k < 0) {
      k = -k;
      m = 1;
    }
    *--b = 0;
    do {
      *--b = digitmap[(int)(k % j)];
    } while(k /= j);

    if(m) {
      *--b = '-';
    }

    return String::create(state, b);
  }

  Array* Fixnum::coerce(STATE, Bignum* other) {
    Array* ary = Array::create(state, 2);

    if(FIXNUM fix = try_as<Fixnum>(Bignum::normalize(state, other))) {
      ary->set(state, 0, fix);
      ary->set(state, 1, this);
    } else {
      ary->set(state, 0, other);
      ary->set(state, 1, Bignum::create(state, this));
    }

    return ary;
  }

  Array* Fixnum::coerce(STATE, FIXNUM other) {
    Array* ary = Array::create(state, 2);

    ary->set(state, 0, other);
    ary->set(state, 1, this);

    return ary;
  }

  void Fixnum::Info::show(STATE, OBJECT self, int level) {
    FIXNUM f = as<Fixnum>(self);
    std::cout << f->to_native() << std::endl;
  }

  void Fixnum::Info::show_simple(STATE, OBJECT self, int level) {
    show(state, self, level);
  }

  void Fixnum::Info::mark(OBJECT t, ObjectMark& mark) { }
}
