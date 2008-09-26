#ifndef RBX_BUILTIN_FIXNUM_HPP
#define RBX_BUILTIN_FIXNUM_HPP

#include "builtin/integer.hpp"
#include "builtin/bignum.hpp"

namespace rubinius {
  class Float;
  class Array;
  class String;

  class Fixnum : public Integer {
  public:
    const static size_t fields = 0;
    const static object_type type = FixnumType;

    static FIXNUM from(native_int);

    native_int         to_native() const;

    int                to_int() const;
    unsigned int       to_uint() const;
    long               to_long() const;
    unsigned long      to_ulong() const;
    long long          to_long_long() const;
    unsigned long long to_ulong_long() const;

    // Ruby.primitive! :fixnum_add
    INTEGER add(STATE, FIXNUM other);

    // Ruby.primitive! :fixnum_add
    INTEGER add(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_add
    Float* add(STATE, Float* other);

    // Ruby.primitive! :fixnum_sub
    INTEGER sub(STATE, FIXNUM other);

    // Ruby.primitive! :fixnum_sub
    INTEGER sub(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_sub
    Float* sub(STATE, Float* other);

    // Ruby.primitive! :fixnum_mul
    INTEGER mul(STATE, FIXNUM other);

    // Ruby.primitive! :fixnum_mul
    INTEGER mul(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_mul
    Float* mul(STATE, Float* other);

    // Ruby.primitive! :fixnum_div
    INTEGER div(STATE, FIXNUM other);

    // Ruby.primitive! :fixnum_div
    INTEGER div(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_div
    Float* div(STATE, Float* other);

    // Ruby.primitive! :fixnum_mod
    INTEGER mod(STATE, FIXNUM other);

    // Ruby.primitive! :fixnum_mod
    INTEGER mod(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_mod
    Float* mod(STATE, Float* other);

    // Ruby.primitive! :fixnum_divmod
    Array* divmod(STATE, FIXNUM other);

    // Ruby.primitive! :fixnum_divmod
    Array* divmod(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_divmod
    Array* divmod(STATE, Float* other);

    // Ruby.primitive :fixnum_neg
    INTEGER neg(STATE);

    // Ruby.primitive! :fixnum_equal
    OBJECT equal(STATE, FIXNUM other);

    // Ruby.primitive! :fixnum_equal
    OBJECT equal(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_equal
    OBJECT equal(STATE, Float* other);

    // Ruby.primitive! :fixnum_compare
    FIXNUM compare(STATE, FIXNUM other);

    // Ruby.primitive! :fixnum_compare
    FIXNUM compare(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_compare
    FIXNUM compare(STATE, Float* other);

    // Ruby.primitive! :fixnum_gt
    OBJECT gt(STATE, FIXNUM other);

    // Ruby.primitive! :fixnum_gt
    OBJECT gt(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_gt
    OBJECT gt(STATE, Float* other);

    // Ruby.primitive! :fixnum_ge
    OBJECT ge(STATE, FIXNUM other);

    // Ruby.primitive! :fixnum_ge
    OBJECT ge(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_ge
    OBJECT ge(STATE, Float* other);

    // Ruby.primitive! :fixnum_lt
    OBJECT lt(STATE, FIXNUM other);

    // Ruby.primitive! :fixnum_lt
    OBJECT lt(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_lt
    OBJECT lt(STATE, Float* other);

    // Ruby.primitive! :fixnum_le
    OBJECT le(STATE, FIXNUM other);

    // Ruby.primitive! :fixnum_le
    OBJECT le(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_le
    OBJECT le(STATE, Float* other);

    // Ruby.primitive :fixnum_left_shift
    INTEGER left_shift(STATE, INTEGER bits);

    // Ruby.primitive :fixnum_right_shift
    INTEGER right_shift(STATE, INTEGER bits);

    // Ruby.primitive :fixnum_size
    INTEGER size(STATE);

    // Ruby.primitive! :fixnum_and
    INTEGER bit_and(STATE, FIXNUM other);

    // Ruby.primitive! :fixnum_and
    INTEGER bit_and(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_and
    INTEGER bit_and(STATE, Float* other);

    // Ruby.primitive! :fixnum_or
    INTEGER bit_or(STATE, FIXNUM other);

    // Ruby.primitive! :fixnum_or
    INTEGER bit_or(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_or
    INTEGER bit_or(STATE, Float* other);

    // Ruby.primitive! :fixnum_xor
    INTEGER bit_xor(STATE, FIXNUM other);

    // Ruby.primitive! :fixnum_xor
    INTEGER bit_xor(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_xor
    INTEGER bit_xor(STATE, Float* other);

    // Ruby.primitive :fixnum_invert
    INTEGER invert(STATE);

    // Ruby.primitive :fixnum_to_f
    Float* to_f(STATE);

    // Ruby.primitive :fixnum_to_s
    String* to_s(STATE, Fixnum* base);

    String* to_s(STATE);

    // Ruby.primitive! :fixnum_coerce
    Array* coerce(STATE, Bignum* other);
    // Ruby.primitive! :fixnum_coerce
    Array* coerce(STATE, FIXNUM other);

    class Info : public TypeInfo {
    public:
      Info(object_type type, bool cleanup = false) : TypeInfo(type, cleanup) { }
      virtual void mark(OBJECT t, ObjectMark& mark);
      virtual void show(STATE, OBJECT self, int level);
      virtual void show_simple(STATE, OBJECT self, int level);
    };
  };

  typedef Fixnum* FIXNUM;
}

#endif
