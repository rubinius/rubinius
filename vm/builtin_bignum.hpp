#ifndef RBX_BUILTIN_BIGNUM_HPP
#define RBX_BUILTIN_BIGNUM_HPP

#include "objects.hpp"

namespace rubinius {
  class Bignum : public BuiltinType {
    public:
    const static size_t fields = 0;
    const static object_type type = BignumType;

    static bool is_a(OBJECT obj) {
      return obj->obj_type == BignumType;
    }

    static void cleanup(STATE, OBJECT obj);
    static void init(STATE);
    static Bignum* create(STATE, native_int num);
    static Bignum* new_unsigned(STATE, unsigned int num);
    static OBJECT  normalize(STATE, Bignum* obj);
    static Bignum* from_ull(STATE, unsigned long long val);
    static Bignum* from_ll(STATE, long long val);
    static OBJECT from_string_detect(STATE, char* str);
    static OBJECT from_string(STATE, const char* str, size_t radix);
    static OBJECT from_double(STATE, double d);
    
    void   debug(STATE);
    OBJECT add(STATE, OBJECT b);
    OBJECT sub(STATE, OBJECT b);
    OBJECT mul(STATE, OBJECT b);
    OBJECT div(STATE, OBJECT b, OBJECT mod);
    OBJECT divmod(STATE, OBJECT b);
    OBJECT mod(STATE, OBJECT b);
    OBJECT bit_and(STATE, OBJECT b);
    OBJECT bit_or(STATE, OBJECT b);
    OBJECT bit_xor(STATE, OBJECT b);
    OBJECT invert(STATE);
    OBJECT neg(STATE);
    OBJECT left_shift(STATE, OBJECT bits);
    OBJECT right_shift(STATE, OBJECT bits);
    OBJECT equal(STATE, OBJECT b);
    OBJECT compare(STATE, OBJECT b);
    OBJECT gt(STATE, OBJECT b);
    OBJECT ge(STATE, OBJECT b);
    OBJECT lt(STATE, OBJECT b);
    OBJECT le(STATE, OBJECT b);
    bool   is_zero(STATE);
    unsigned long to_int(STATE);
    int    to_i(STATE);
    unsigned int to_ui(STATE);
    unsigned long long to_ull(STATE);
    long long to_ll(STATE);
    OBJECT to_s(STATE, OBJECT radix);
    void   into_string(STATE, size_t radix, char* buf, size_t sz);
    double to_double(STATE);
    OBJECT size(STATE);
    hashval hash_bignum(STATE);

    class Info : public TypeInfo {
    public:
      Info(Class* cls) : TypeInfo(cls) { }
      virtual void cleanup(OBJECT obj);
    };
  };

}

#endif
