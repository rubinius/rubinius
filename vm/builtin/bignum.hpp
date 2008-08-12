#ifndef RBX_BUILTIN_BIGNUM_HPP
#define RBX_BUILTIN_BIGNUM_HPP

#include "objects.hpp" // TODO: break up objects.hpp
#include "builtin/integer.hpp"

namespace rubinius {
  class Array;
  class String;
  class Float;

  class Bignum : public Integer {
    public:
    const static size_t fields = 0;
    const static object_type type = BignumType;

    static void cleanup(STATE, OBJECT obj);
    static void init(STATE);

    static Bignum* from(STATE, int num);
    static Bignum* from(STATE, long num);
    static Bignum* from(STATE, unsigned long);
    static Bignum* from(STATE, long long val);
    static Bignum* from(STATE, unsigned long long val);

    native_int         to_native();

    int                to_int();
    unsigned int       to_uint();
    long               to_long();
    unsigned long      to_ulong();
    long long          to_long_long();
    unsigned long long to_ulong_long();

    static INTEGER normalize(STATE, Bignum* obj);
    static INTEGER from_string_detect(STATE, const char* str);
    static INTEGER from_string(STATE, const char* str, size_t radix);
    static INTEGER from_double(STATE, double d);

    // Ruby.primitive :bignum_from_double
    static INTEGER from_double(STATE, Float* f); // FIX: this is horrible to overload against a non-primitive

    void   debug(STATE);
    // Ruby.primitive! :bignum_add
    INTEGER add(STATE, FIXNUM b);
    // Ruby.primitive! :bignum_add
    INTEGER add(STATE, Bignum* b);
    // Ruby.primitive! :bignum_add
    Float*  add(STATE, Float* b);

    // Ruby.primitive! :bignum_sub
    INTEGER sub(STATE, FIXNUM b);
    // Ruby.primitive! :bignum_sub
    INTEGER sub(STATE, Bignum* b);
    // Ruby.primitive! :bignum_sub
    Float*  sub(STATE, Float* b);

    // Ruby.primitive! :bignum_mul
    INTEGER mul(STATE, FIXNUM b);
    // Ruby.primitive! :bignum_mul
    INTEGER mul(STATE, Bignum* b);
    // Ruby.primitive! :bignum_mul
    Float*  mul(STATE, Float* b);

    INTEGER divide(STATE, FIXNUM b, INTEGER* mod);
    INTEGER divide(STATE, Bignum* b, INTEGER* mod);

    // Ruby.primitive! :bignum_div
    INTEGER div(STATE, FIXNUM b);
    // Ruby.primitive! :bignum_div
    INTEGER div(STATE, Bignum* b);
    // Ruby.primitive! :bignum_div
    Float*  div(STATE, Float* b);

    // Ruby.primitive! :bignum_divmod
    Array* divmod(STATE, FIXNUM b);
    // Ruby.primitive! :bignum_divmod
    Array* divmod(STATE, Bignum* b);
    // Ruby.primitive! :bignum_divmod
    Array* divmod(STATE, Float* b);

    // Ruby.primitive! :bignum_mod
    INTEGER mod(STATE, FIXNUM b);
    // Ruby.primitive! :bignum_mod
    INTEGER mod(STATE, Bignum* b);
    // Ruby.primitive! :bignum_mod
    Float* mod(STATE, Float* b);

    // Ruby.primitive :bignum_invert
    INTEGER invert(STATE);
    // Ruby.primitive :bignum_neg
    INTEGER neg(STATE);

    // Ruby.primitive! :bignum_equal
    OBJECT equal(STATE, FIXNUM b);
    // Ruby.primitive! :bignum_equal
    OBJECT equal(STATE, Bignum* b);
    // Ruby.primitive! :bignum_equal
    OBJECT equal(STATE, Float* b);

    // Ruby.primitive! :bignum_compare
    FIXNUM compare(STATE, FIXNUM b);
    // Ruby.primitive! :bignum_compare
    FIXNUM compare(STATE, Bignum* b);
    // Ruby.primitive! :bignum_compare
    FIXNUM compare(STATE, Float* b);

    // Ruby.primitive! :bignum_and
    INTEGER bit_and(STATE, INTEGER b);
    // Ruby.primitive! :bignum_and
    INTEGER bit_and(STATE, Float* b);

    // Ruby.primitive! :bignum_or
    INTEGER bit_or(STATE, INTEGER b);
    // Ruby.primitive! :bignum_or
    INTEGER bit_or(STATE, Float* b);

    // Ruby.primitive! :bignum_xor
    INTEGER bit_xor(STATE, INTEGER b);
    // Ruby.primitive! :bignum_xor
    INTEGER bit_xor(STATE, Float* b);

    // Ruby.primitive :bignum_left_shift
    INTEGER left_shift(STATE, INTEGER bits);
    // Ruby.primitive :bignum_right_shift
    INTEGER right_shift(STATE, INTEGER bits);

    // Ruby.primitive! :bignum_gt
    OBJECT gt(STATE, FIXNUM b);
    // Ruby.primitive! :bignum_gt
    OBJECT gt(STATE, Bignum* b);
    // Ruby.primitive! :bignum_gt
    OBJECT gt(STATE, Float* b);

    // Ruby.primitive! :bignum_ge
    OBJECT ge(STATE, FIXNUM b);
    // Ruby.primitive! :bignum_ge
    OBJECT ge(STATE, Bignum* b);
    // Ruby.primitive! :bignum_ge
    OBJECT ge(STATE, Float* b);

    // Ruby.primitive! :bignum_lt
    OBJECT lt(STATE, FIXNUM b);
    // Ruby.primitive! :bignum_lt
    OBJECT lt(STATE, Bignum* b);
    // Ruby.primitive! :bignum_lt
    OBJECT lt(STATE, Float* b);

    // Ruby.primitive! :bignum_le
    OBJECT le(STATE, FIXNUM b);
    // Ruby.primitive! :bignum_le
    OBJECT le(STATE, Bignum* b);
    // Ruby.primitive! :bignum_le
    OBJECT le(STATE, Float* b);

    // Ruby.primitive :bignum_to_f
    Float* to_f(STATE);

    // Ruby.primitive :bignum_to_s
    String* to_s(STATE, INTEGER radix);
    void   into_string(STATE, size_t radix, char* buf, size_t sz);
    double to_double(STATE);

    // Ruby.primitive :bignum_size
    INTEGER size(STATE);
    hashval hash_bignum(STATE);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(OBJECT t, ObjectMark& mark);
      virtual void cleanup(OBJECT obj);
    };
  };

}

#endif
