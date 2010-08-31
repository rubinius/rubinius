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
    const static object_type type = FixnumType;

    static void init(STATE);

    /* WARNING. Do not use this version if +num+ has the chance of being
     * greater than FIXNUM_MAX or less than FIXNUM_MIN. */
    static Fixnum* from(native_int num) {
      return (Fixnum*)APPLY_FIXNUM_TAG(num);
    }

    native_int to_native() const {
      return STRIP_FIXNUM_TAG(this);
    }

    int                to_int() const;
    unsigned int       to_uint() const;
    long               to_long() const;
    unsigned long      to_ulong() const;
    long long          to_long_long() const;
    unsigned long long to_ulong_long() const;

    bool positive_p() const;

    // Ruby.primitive :fixnum_s_eqq
    static Object* is_fixnum(STATE, Object* obj) {
      return obj->fixnum_p() ? Qtrue : Qfalse;
    }

    // Ruby.primitive! :fixnum_add
    Integer* add(STATE, Fixnum* other) {
      native_int r = to_native() + other->to_native();
      if(r > FIXNUM_MAX || r < FIXNUM_MIN) {
        return Bignum::from(state, r);
      } else {
        return Fixnum::from(r);
      }
    }

    // Ruby.primitive! :fixnum_add
    Integer* add(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_add
    Float* add(STATE, Float* other);

    // Ruby.primitive! :fixnum_sub
    Integer* sub(STATE, Fixnum* other) {
      native_int r = to_native() - other->to_native();
      if(r > FIXNUM_MAX || r < FIXNUM_MIN) {
        return Bignum::from(state, r);
      } else {
        return Fixnum::from(r);
      }
    }

    // Ruby.primitive! :fixnum_sub
    Integer* sub(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_sub
    Float* sub(STATE, Float* other);

    // Ruby.primitive! :fixnum_mul
    Integer* mul(STATE, Fixnum* other);

    // Ruby.primitive! :fixnum_mul
    Integer* mul(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_mul
    Float* mul(STATE, Float* other);

    // Ruby.primitive! :fixnum_div
    Integer* div(STATE, Fixnum* other);

    // Ruby.primitive! :fixnum_div
    Integer* div(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_mod
    Integer* mod(STATE, Fixnum* other);

    // Ruby.primitive! :fixnum_mod
    Integer* mod(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_mod
    Float* mod(STATE, Float* other);

    // Ruby.primitive! :fixnum_divmod
    Array* divmod(STATE, Fixnum* other);

    // Ruby.primitive! :fixnum_divmod
    Array* divmod(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_divmod
    Array* divmod(STATE, Float* other);

    // Ruby.primitive :fixnum_neg
    Integer* neg(STATE);

    // Ruby.primitive! :fixnum_pow
    Object* pow(STATE, Fixnum *exponent);

    // Ruby.primitive! :fixnum_pow
    Object* pow(STATE, Bignum *exponent);

    // Ruby.primitive! :fixnum_pow
    Float* pow(STATE, Float *exponent);

    // Ruby.primitive! :fixnum_equal
    Object* equal(STATE, Fixnum* other);

    // Ruby.primitive! :fixnum_equal
    Object* equal(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_equal
    Object* equal(STATE, Float* other);

    // Ruby.primitive! :fixnum_compare
    Object* compare(STATE, Fixnum* other);

    // Ruby.primitive! :fixnum_compare
    Object* compare(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_compare
    Object* compare(STATE, Float* other);

    // Ruby.primitive! :fixnum_gt
    Object* gt(STATE, Fixnum* other) {
      return to_native() > other->to_native() ? Qtrue : Qfalse;
    }

    // Ruby.primitive! :fixnum_gt
    Object* gt(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_gt
    Object* gt(STATE, Float* other);

    // Ruby.primitive! :fixnum_ge
    Object* ge(STATE, Fixnum* other);

    // Ruby.primitive! :fixnum_ge
    Object* ge(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_ge
    Object* ge(STATE, Float* other);

    // Ruby.primitive! :fixnum_lt
    Object* lt(STATE, Fixnum* other) {
      return to_native() < other->to_native() ? Qtrue : Qfalse;
    }

    // Ruby.primitive! :fixnum_lt
    Object* lt(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_lt
    Object* lt(STATE, Float* other);

    // Ruby.primitive! :fixnum_le
    Object* le(STATE, Fixnum* other);

    // Ruby.primitive! :fixnum_le
    Object* le(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_le
    Object* le(STATE, Float* other);

    // Ruby.primitive :fixnum_left_shift
    Integer* left_shift(STATE, Fixnum* bits);

    // Ruby.primitive :fixnum_right_shift
    Integer* right_shift(STATE, Fixnum* bits);

    // Ruby.primitive :fixnum_size
    Integer* size(STATE);

    // Ruby.primitive! :fixnum_and
    Integer* bit_and(STATE, Fixnum* other);

    // Ruby.primitive! :fixnum_and
    Integer* bit_and(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_and
    Integer* bit_and(STATE, Float* other);

    // Ruby.primitive! :fixnum_or
    Integer* bit_or(STATE, Fixnum* other);

    // Ruby.primitive! :fixnum_or
    Integer* bit_or(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_or
    Integer* bit_or(STATE, Float* other);

    // Ruby.primitive! :fixnum_xor
    Integer* bit_xor(STATE, Fixnum* other);

    // Ruby.primitive! :fixnum_xor
    Integer* bit_xor(STATE, Bignum* other);

    // Ruby.primitive! :fixnum_xor
    Integer* bit_xor(STATE, Float* other);

    // Ruby.primitive :fixnum_invert
    Integer* invert(STATE);

    // Ruby.primitive :fixnum_to_f
    Float* to_f(STATE);

    // Ruby.primitive :fixnum_to_s
    String* to_s(STATE, Fixnum* base);

    String* to_s(STATE);

    // Ruby.primitive! :fixnum_coerce
    Array* coerce(STATE, Bignum* other);
    // Ruby.primitive! :fixnum_coerce
    Array* coerce(STATE, Fixnum* other);

    class Info : public TypeInfo {
    public:
      Info(object_type type)
        : TypeInfo(type)
      {
        allow_user_allocate = false;
      }

      virtual void mark(Object* t, ObjectMark& mark);
      virtual void show(STATE, Object* self, int level);
      virtual void show_simple(STATE, Object* self, int level);
      virtual void auto_mark(Object* obj, ObjectMark& mark) {}
    };
  };

}

#endif
