#ifndef RBX_BUILTIN_FIXNUM_HPP
#define RBX_BUILTIN_FIXNUM_HPP

namespace rubinius {

  class Fixnum : public Integer {
  public:
    const static size_t fields = 0;
    const static object_type type = FixnumType;

    // Ruby.primitive! :fixnum_add
    INTEGER add(STATE, FIXNUM other) {
      native_int r = n2i() + other->n2i();
      if(r > FIXNUM_MAX || r < FIXNUM_MIN) {
        return Bignum::create(state, r);
      } else {
        return Object::i2n(state, r);
      }
    }

    // Ruby.primitive! :fixnum_add
    INTEGER add(STATE, Bignum* other) {
      return other->add(state, this);
    }

    // Ruby.primitive! :fixnum_add
    Float* add(STATE, Float* other) {
      return other->add(state, this);
    }

    // Ruby.primitive! :fixnum_sub
    INTEGER sub(STATE, FIXNUM other) {
      native_int r = n2i() - other->n2i();
      if(r > FIXNUM_MAX || r < FIXNUM_MIN) {
        return Bignum::create(state, r);
      } else {
        return Object::i2n(state, r);
      }
    }
    
    // Ruby.primitive! :fixnum_sub
    INTEGER sub(STATE, Bignum* other) {
      return as<Bignum>(other->neg(state))->add(state, this);
    }

    // Ruby.primitive! :fixnum_sub
    Float* sub(STATE, Float* other) {
      return Float::coerce(state, this)->sub(state, other);
    }

    // Ruby.primitive! :fixnum_mul
    INTEGER mul(STATE, FIXNUM other) {
      native_int a  = n2i();
      native_int b  = other->n2i();
      
      if(a == 0 || b == 0) return Object::i2n(0);
      
      if(a > 0) {
        if(b > 0) {
          if(a > (FIXNUM_MAX / b)) {
            return Bignum::create(state, a)->mul(state, other);
          } 
        } else {
          if (b < (FIXNUM_MIN / a)) {
            return Bignum::create(state, a)->mul(state, other);
          }
        }
      } else {
        if(b > 0){
          if(a < (FIXNUM_MIN / b)) {
            return Bignum::create(state, a)->mul(state, other);
          }
        } else {
          if(b < (FIXNUM_MAX / a)) {
            return Bignum::create(state, a)->mul(state, other);
          }
        }
      }
      
      return Object::i2n(state, n2i() * other->n2i());
    }

    // Ruby.primitive! :fixnum_mul
    INTEGER mul(STATE, Bignum* other) {
      return other->mul(state, this);
    }

    // Ruby.primitive! :fixnum_mul
    Float* mul(STATE, Float* other) {
      return other->mul(state, this);
    }

    // Ruby.primitive! :fixnum_div
    INTEGER div(STATE, FIXNUM other) {
      native_int numerator = n2i();
      native_int denominator = other->n2i();
      native_int quotient = numerator / denominator;
      if(quotient < 0 && quotient * denominator != numerator) --quotient;
      return Object::i2n(state, quotient);
    }

    // Ruby.primitive! :fixnum_div
    INTEGER div(STATE, Bignum* other) {
      return Bignum::create(state, n2i())->div(state, other);
    }

    // Ruby.primitive! :fixnum_div
    Float* div(STATE, Float* other) {
      return Float::coerce(state, this)->div(state, other);
    }

    // Ruby.primitive! :fixnum_mod
    INTEGER mod(STATE, FIXNUM other) {
      native_int numerator = n2i();
      native_int denominator = other->n2i();
      native_int quotient = div(state, other)->n2i();
      return Object::i2n(state, numerator - denominator * quotient);
    }

    // Ruby.primitive! :fixnum_mod
    INTEGER mod(STATE, Bignum* other) {
      return Bignum::create(state, n2i())->mod(state, other);
    }

    // Ruby.primitive! :fixnum_mod
    Float* mod(STATE, Float* other) {
      return Float::create(state, n2i())->mod(state, other);
    }

    // Ruby.primitive! :fixnum_divmod
    Array* divmod(STATE, FIXNUM other) {
      native_int numerator = n2i();
      native_int denominator = other->n2i();
      native_int fraction = div(state, other)->n2i();
      Array* ary = Array::create(state, 2);
      ary->set(state, 0, Object::i2n(fraction));
      ary->set(state, 1, Object::i2n(numerator - denominator * fraction));
      return ary;
    }

    // Ruby.primitive! :fixnum_divmod
    Array* divmod(STATE, Bignum* other) {
      return Bignum::create(state, n2i())->divmod(state, other);
    }

    // Ruby.primitive! :fixnum_divmod
    Array* divmod(STATE, Float* other) {
      return Float::create(state, n2i())->divmod(state, other);
    }

    // Ruby.primitive! :fixnum_equal
    OBJECT equal(STATE, FIXNUM other) {
      return n2i() == other->n2i() ? Qtrue : Qfalse;
    }

    // Ruby.primitive! :fixnum_equal
    OBJECT equal(STATE, Bignum* other) {
      return other->equal(state, this);
    }

    // Ruby.primitive! :fixnum_equal
    OBJECT equal(STATE, Float* other) {
      return (double)n2i() == other->val ? Qtrue : Qfalse;
    }

    // Ruby.primitive! :fixnum_gt
    OBJECT gt(STATE, FIXNUM other) {
      return n2i() > other->n2i() ? Qtrue : Qfalse;
    }

    // Ruby.primitive! :fixnum_gt
    OBJECT gt(STATE, Bignum* other) {
      return other->lt(state, this);
    }

    // Ruby.primitive! :fixnum_gt
    OBJECT gt(STATE, Float* other) {
      return (double) n2i() > other->val ? Qtrue : Qfalse;
    }

    // Ruby.primitive! :fixnum_ge
    OBJECT ge(STATE, FIXNUM other) {
      return n2i() >= other->n2i() ? Qtrue : Qfalse;      
    }

    // Ruby.primitive! :fixnum_ge
    OBJECT ge(STATE, Bignum* other) {
      return other->le(state, this);
    }

    // Ruby.primitive! :fixnum_ge
    OBJECT ge(STATE, Float* other) {
      return (double) n2i() >= other->val ? Qtrue : Qfalse;
    }

    // Ruby.primitive! :fixnum_lt
    OBJECT lt(STATE, FIXNUM other) {
      return n2i() < other->n2i() ? Qtrue : Qfalse;      
    }

    // Ruby.primitive! :fixnum_lt
    OBJECT lt(STATE, Bignum* other) {
      return other->gt(state, this);
    }

    // Ruby.primitive! :fixnum_lt
    OBJECT lt(STATE, Float* other) {
      return (double) n2i() < other->val ? Qtrue : Qfalse;
    }

    // Ruby.primitive! :fixnum_le
    OBJECT le(STATE, FIXNUM other) {
      return n2i() <= other->n2i() ? Qtrue : Qfalse;      
    }

    // Ruby.primitive! :fixnum_le
    OBJECT le(STATE, Bignum* other) {
      return other->ge(state, this);
    }

    // Ruby.primitive! :fixnum_le
    OBJECT le(STATE, Float* other) {
      return (double) n2i() <= other->val ? Qtrue : Qfalse;
    }

    // Ruby.primitive :fixnum_left_shift
    INTEGER left_shift(STATE, INTEGER bits) {
      native_int shift = bits->n2i();
      if(shift < 0) {
        return right_shift(state, Object::i2n(-shift));
      }
      
      native_int self = n2i();
      
      if(shift > (native_int)FIXNUM_WIDTH || self >> ((native_int)FIXNUM_WIDTH - shift) > 0) {
        return Bignum::create(state, self)->left_shift(state, bits);
      }
      
      return Object::i2n(self << shift);
    }

    // Ruby.primitive :fixnum_right_shift
    INTEGER right_shift(STATE, INTEGER bits) {
      native_int shift = bits->n2i();
      if(shift < 0) {
        return left_shift(state, Object::i2n(-shift));
      }
      
      native_int self = n2i();
      
      native_int a = self >> shift;
      return Object::i2n(a); 
    }

    native_int to_nint() {
      return STRIP_TAG(this);
    }

    INTEGER size() {
      return Object::i2n(sizeof(native_int));
    }

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
