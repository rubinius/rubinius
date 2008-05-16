#ifndef RBX_BUILTIN_BIGNUM_HPP
#define RBX_BUILTIN_BIGNUM_HPP

#include "objects.hpp"

namespace rubinius {
  class Array;

  class Bignum : public Integer {
    public:
    const static size_t fields = 0;
    const static object_type type = BignumType;

    static void cleanup(STATE, OBJECT obj);
    static void init(STATE);
    static Bignum* create(STATE, native_int num);
    static Bignum* new_unsigned(STATE, unsigned int num);
    static INTEGER normalize(STATE, Bignum* obj);
    static Bignum* from_ull(STATE, unsigned long long val);
    static Bignum* from_ll(STATE, long long val);
    static OBJECT from_string_detect(STATE, char* str);
    static OBJECT from_string(STATE, const char* str, size_t radix);
    static OBJECT from_double(STATE, double d);
    
    void   debug(STATE);
    INTEGER add(STATE, INTEGER b);
    INTEGER sub(STATE, INTEGER b);
    INTEGER mul(STATE, INTEGER b);
    INTEGER div(STATE, INTEGER b, INTEGER mod);
    Array* divmod(STATE, INTEGER b);
    INTEGER mod(STATE, INTEGER b);
    INTEGER bit_and(STATE, INTEGER b);
    INTEGER bit_or(STATE, INTEGER b);
    INTEGER bit_xor(STATE, INTEGER b);
    INTEGER invert(STATE);
    INTEGER neg(STATE);
    INTEGER left_shift(STATE, INTEGER bits);
    INTEGER right_shift(STATE, INTEGER bits);
    FIXNUM compare(STATE, INTEGER b);
    OBJECT equal(STATE, INTEGER b);
    OBJECT gt(STATE, INTEGER b);
    OBJECT ge(STATE, INTEGER b);
    OBJECT lt(STATE, INTEGER b);
    OBJECT le(STATE, INTEGER b);
    bool   is_zero(STATE);
    int    to_int(STATE);
    int    to_i(STATE);
    native_int to_nint();
    unsigned int to_ui(STATE);
    unsigned long long to_ull(STATE);
    long long to_ll(STATE);
    OBJECT to_s(STATE, INTEGER radix);
    void   into_string(STATE, size_t radix, char* buf, size_t sz);
    double to_double(STATE);
    OBJECT size(STATE);
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
