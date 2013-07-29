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
      return reinterpret_cast<Fixnum*>(APPLY_FIXNUM_TAG(num));
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

    // Rubinius.primitive+ :fixnum_s_eqq
    static Object* is_fixnum(STATE, Object* obj) {
      return RBOOL(obj->fixnum_p());
    }

    // Rubinius.primitive! :fixnum_add
    Integer* add(STATE, Fixnum* other) {
      native_int r = to_native() + other->to_native();
      if(r > FIXNUM_MAX || r < FIXNUM_MIN) {
        return Bignum::from(state, r);
      } else {
        return from(r);
      }
    }

    // Rubinius.primitive! :fixnum_add
    Integer* add(STATE, Bignum* other);

    // Rubinius.primitive! :fixnum_add
    Float* add(STATE, Float* other);

    // Rubinius.primitive! :fixnum_sub
    Integer* sub(STATE, Fixnum* other) {
      native_int r = to_native() - other->to_native();
      if(r > FIXNUM_MAX || r < FIXNUM_MIN) {
        return Bignum::from(state, r);
      } else {
        return from(r);
      }
    }

    // Rubinius.primitive! :fixnum_sub
    Integer* sub(STATE, Bignum* other);

    // Rubinius.primitive! :fixnum_sub
    Float* sub(STATE, Float* other);

    // Rubinius.primitive! :fixnum_mul
    Integer* mul(STATE, Fixnum* other);

    // Rubinius.primitive! :fixnum_mul
    Integer* mul(STATE, Bignum* other);

    // Rubinius.primitive! :fixnum_mul
    Float* mul(STATE, Float* other);

    // Rubinius.primitive! :fixnum_div
    Integer* div(STATE, Fixnum* other);

    // Rubinius.primitive! :fixnum_div
    Integer* div(STATE, Bignum* other);

    // Rubinius.primitive! :fixnum_div
    Float* div(STATE, Float* other);

    // Rubinius.primitive! :fixnum_mod
    Integer* mod(STATE, Fixnum* other);

    // Rubinius.primitive! :fixnum_mod
    Integer* mod(STATE, Bignum* other);

    // Rubinius.primitive! :fixnum_mod
    Float* mod(STATE, Float* other);

    // Rubinius.primitive! :fixnum_divmod
    Array* divmod(STATE, Fixnum* other);

    // Rubinius.primitive! :fixnum_divmod
    Array* divmod(STATE, Bignum* other);

    // Rubinius.primitive! :fixnum_divmod
    Array* divmod(STATE, Float* other);

    // Rubinius.primitive+ :fixnum_neg
    Integer* neg(STATE);

    // Rubinius.primitive! :fixnum_pow
    Object* pow(STATE, Fixnum *exponent);

    // Rubinius.primitive! :fixnum_pow
    Object* pow(STATE, Bignum *exponent);

    // Rubinius.primitive! :fixnum_pow
    Object* pow(STATE, Float *exponent);

    // Rubinius.primitive! :fixnum_equal
    Object* equal(STATE, Fixnum* other);

    // Rubinius.primitive! :fixnum_equal
    Object* equal(STATE, Bignum* other);

    // Rubinius.primitive! :fixnum_equal
    Object* equal(STATE, Float* other);

    // Rubinius.primitive! :fixnum_compare
    Object* compare(STATE, Fixnum* other);

    // Rubinius.primitive! :fixnum_compare
    Object* compare(STATE, Bignum* other);

    // Rubinius.primitive! :fixnum_compare
    Object* compare(STATE, Float* other);

    // Rubinius.primitive! :fixnum_gt
    Object* gt(STATE, Fixnum* other) {
      return RBOOL(to_native() > other->to_native());
    }

    // Rubinius.primitive! :fixnum_gt
    Object* gt(STATE, Bignum* other);

    // Rubinius.primitive! :fixnum_gt
    Object* gt(STATE, Float* other);

    // Rubinius.primitive! :fixnum_ge
    Object* ge(STATE, Fixnum* other);

    // Rubinius.primitive! :fixnum_ge
    Object* ge(STATE, Bignum* other);

    // Rubinius.primitive! :fixnum_ge
    Object* ge(STATE, Float* other);

    // Rubinius.primitive! :fixnum_lt
    Object* lt(STATE, Fixnum* other) {
      return RBOOL(to_native() < other->to_native());
    }

    // Rubinius.primitive! :fixnum_lt
    Object* lt(STATE, Bignum* other);

    // Rubinius.primitive! :fixnum_lt
    Object* lt(STATE, Float* other);

    // Rubinius.primitive! :fixnum_le
    Object* le(STATE, Fixnum* other);

    // Rubinius.primitive! :fixnum_le
    Object* le(STATE, Bignum* other);

    // Rubinius.primitive! :fixnum_le
    Object* le(STATE, Float* other);

    // Rubinius.primitive :fixnum_left_shift
    Integer* left_shift(STATE, Fixnum* bits);

    // Rubinius.primitive :fixnum_right_shift
    Integer* right_shift(STATE, Fixnum* bits);

    // Rubinius.primitive+ :fixnum_size
    Integer* size(STATE);

    // Rubinius.primitive! :fixnum_and
    Integer* bit_and(STATE, Fixnum* other);

    // Rubinius.primitive! :fixnum_and
    Integer* bit_and(STATE, Bignum* other);

    // Rubinius.primitive! :fixnum_and
    Integer* bit_and(STATE, Float* other);

    // Rubinius.primitive! :fixnum_or
    Integer* bit_or(STATE, Fixnum* other);

    // Rubinius.primitive! :fixnum_or
    Integer* bit_or(STATE, Bignum* other);

    // Rubinius.primitive! :fixnum_or
    Integer* bit_or(STATE, Float* other);

    // Rubinius.primitive! :fixnum_xor
    Integer* bit_xor(STATE, Fixnum* other);

    // Rubinius.primitive! :fixnum_xor
    Integer* bit_xor(STATE, Bignum* other);

    // Rubinius.primitive! :fixnum_xor
    Integer* bit_xor(STATE, Float* other);

    // Rubinius.primitive+ :fixnum_invert
    Integer* invert(STATE);

    // Rubinius.primitive :fixnum_to_f
    Float* to_f(STATE);

    // Rubinius.primitive :fixnum_to_s
    String* to_s(STATE, Fixnum* base);

    String* to_s(STATE);

    // Rubinius.primitive! :fixnum_coerce
    Array* coerce(STATE, Bignum* other);
    // Rubinius.primitive! :fixnum_coerce
    Array* coerce(STATE, Fixnum* other);

    Fixnum* fixnum_hash() {
#ifdef _LP64
      uintptr_t key = reinterpret_cast<uintptr_t>(this);
      key = (~key) + (key << 21); // key = (key << 21) - key - 1;
      key = key ^ (key >> 24);
      key = (key + (key << 3)) + (key << 8); // key * 265
      key = key ^ (key >> 14);
      key = (key + (key << 2)) + (key << 4); // key * 21
      key = key ^ (key >> 28);
      key = key + (key << 31);
      return from(key & FIXNUM_MAX);
#else
      // See http://burtleburtle.net/bob/hash/integer.html
      uint32_t a = (uint32_t)this;
      a = (a+0x7ed55d16) + (a<<12);
      a = (a^0xc761c23c) ^ (a>>19);
      a = (a+0x165667b1) + (a<<5);
      a = (a+0xd3a2646c) ^ (a<<9);
      a = (a+0xfd7046c5) + (a<<3);
      a = (a^0xb55a4f09) ^ (a>>16);
      return from(a & FIXNUM_MAX);
#endif
    }

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
