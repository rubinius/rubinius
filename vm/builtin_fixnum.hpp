#ifndef RBX_BUILTIN_FIXNUM_HPP
#define RBX_BUILTIN_FIXNUM_HPP

namespace rubinius {

  class Fixnum : public Integer {
  public:
    const static size_t fields = 0;
    const static object_type type = FixnumType;

    // Ruby.primitive :fixnum_add
    INTEGER add(STATE, FIXNUM other) {
      native_int r = n2i() + other->n2i();
      if(r > FIXNUM_MAX || r < FIXNUM_MIN) {
        return Bignum::create(state, r);
      } else {
        return Object::i2n(state, r);
      }
    }

    INTEGER add(STATE, Bignum* other) {
      return other->add(state, this);
    }

    INTEGER sub(STATE, FIXNUM other) {
      native_int r = n2i() - other->n2i();
      if(r > FIXNUM_MAX || r < FIXNUM_MIN) {
        return Bignum::create(state, r);
      } else {
        return Object::i2n(state, r);
      }
    }
    
    INTEGER sub(STATE, Bignum* other) {
      return as<Bignum>(other->neg(state))->add(state, this);
    }

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

    INTEGER mul(STATE, Bignum* other) {
      return other->mul(state, this);
    }

    INTEGER div(STATE, FIXNUM other) {
      native_int numerator = n2i();
      native_int denominator = other->n2i();
      native_int quotient = numerator / denominator;
      if(quotient < 0 && quotient * denominator != numerator) --quotient;
      return Object::i2n(state, quotient);
    }

    INTEGER mod(STATE, FIXNUM other) {
      native_int numerator = n2i();
      native_int denominator = other->n2i();
      native_int quotient = div(state, other)->n2i();
      return Object::i2n(state, numerator - denominator * quotient);
    }

    Array* divmod(STATE, FIXNUM other) {
      native_int numerator = n2i();
      native_int denominator = other->n2i();
      native_int fraction = div(state, other)->n2i();
      Array* ary = Array::create(state, 2);
      ary->set(state, 0, Object::i2n(fraction));
      ary->set(state, 1, Object::i2n(numerator - denominator * fraction));
      return ary;
    }

    native_int to_nint() {
      return STRIP_TAG(this);
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
      throw new TypeError(obj->obj_type, obj, "can't be cast as an Integer");
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
