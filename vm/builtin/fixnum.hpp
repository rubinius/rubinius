#ifndef RBX_BUILTIN_FIXNUM_HPP
#define RBX_BUILTIN_FIXNUM_HPP

#include "builtin/string.hpp"

namespace rubinius {

  class Fixnum : public Integer {
  public:
    const static size_t fields = 0;
    const static object_type type = FixnumType;

    // Ruby.primitive! :add
    INTEGER add(STATE, FIXNUM other);

    // Ruby.primitive! :add
    INTEGER add(STATE, Bignum* other);

    // Ruby.primitive! :add
    Float* add(STATE, Float* other);

    // Ruby.primitive! :sub
    INTEGER sub(STATE, FIXNUM other);

    // Ruby.primitive! :sub
    INTEGER sub(STATE, Bignum* other);

    // Ruby.primitive! :sub
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

    native_int to_nint();

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
    String* to_s(STATE);
  };

  typedef Fixnum* FIXNUM;

  /* See t1 */
  template <>
    static bool kind_of<Integer>(OBJECT obj) {
      return obj->fixnum_p() || (obj->reference_p() && obj->obj_type == Bignum::type);
    }

  /* For some reason, the as<> template doesn't pick up the specialized kind_of<>, until
   * we figure out why, just special as<> too. */
  template <>
    static INTEGER as<Integer>(OBJECT obj) {
      if(kind_of<Integer>(obj)) return (Integer*)obj;
      throw TypeError(obj->obj_type, obj, "can't be cast as an Integer");
    }

  template <>
    static bool kind_of<Fixnum>(OBJECT obj) {
      return obj->fixnum_p();
    }
  
  native_int Integer::n2i() {
    if(fixnum_p()) {
      return ((FIXNUM)this)->to_nint();
    }

    return as<Bignum>(this)->to_nint();
  }
}

#endif
