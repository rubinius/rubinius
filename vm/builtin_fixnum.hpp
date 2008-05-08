#ifndef RBX_BUILTIN_FIXNUM_HPP
#define RBX_BUILTIN_FIXNUM_HPP

namespace rubinius {

  class Fixnum : public Integer {
  public:
    const static size_t fields = 0;
    const static object_type type = FixnumType;

    // Ruby.primitive :fixnum_add
    INTEGER add(STATE, FIXNUM other) {
      return Object::i2n(state, n2i() + other->n2i());
    }

    INTEGER sub(STATE, FIXNUM other) {
      return Object::i2n(state, n2i() - other->n2i());
    }

    INTEGER multiply(STATE, FIXNUM other) {
      return Object::i2n(state, n2i() * other->n2i());
    }

    INTEGER divide(STATE, FIXNUM other) {
      return Object::i2n(state, n2i() / other->n2i());
    }

    INTEGER modulo(STATE, FIXNUM other) {
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
