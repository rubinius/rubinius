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

    INTEGER sub(STATE, FIXNUM other) {
      native_int r = n2i() - other->n2i();
      if(r > FIXNUM_MAX || r < FIXNUM_MIN) {
        return Bignum::create(state, r);
      } else {
        return Object::i2n(state, r);
      }
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
          if (a < (FIXNUM_MIN / b)) {
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

    INTEGER div(STATE, FIXNUM other) {
      return Object::i2n(state, n2i() / other->n2i());
    }

    INTEGER mod(STATE, FIXNUM other) {
      return Object::i2n(state, n2i() % other->n2i());
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
