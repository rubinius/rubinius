#include "prelude.hpp"
#include "object.hpp"
#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

namespace rubinius {
  Float* Float::create(STATE, double val) {
    Float* flt = (Float*)state->new_struct(G(floatpoint), sizeof(Float));
    flt->val = val;
    return flt;
  }

  OBJECT Float::compare(STATE, Float* other) {
    double b = other->to_double();
    if(val < b) {
      return Object::i2n(-1);
    } else if(val > b) {
      return Object::i2n(1);
    } else {
      return Object::i2n(0);
    }
  }

  Float* Float::coerce(STATE, OBJECT value) {
    if(value->fixnum_p()) {
      return Float::create(state, (double)(as<Fixnum>(value)->to_nint()));
    } else if(kind_of<Bignum>(value)) {
      return Float::create(state, as<Bignum>(value)->to_double(state));
    }

    /* FIXME this used to just return value, but this wants to coerce, so
     * we give a default float value of 0.0 back instead. */
    return Float::create(state, 0.0);
  }

  Float* Float::add(STATE, Float* other) {
    return Float::create(state, this->val + other->val);
  }

  Float* Float::add(STATE, INTEGER other) {
    return Float::create(state, this->val + Float::coerce(state, other)->val);
  }

  Float* Float::sub(STATE, Float* other) {
    return Float::create(state, this->val - other->val);
  }

  Float* Float::sub(STATE, INTEGER other) {
    return Float::create(state, this->val - Float::coerce(state, other)->val);
  }

  Float* Float::mul(STATE, Float* other) {
    return Float::create(state, this->val * other->val);
  }

  Float* Float::mul(STATE, INTEGER other) {
    return Float::create(state, this->val * Float::coerce(state, other)->val);
  }

  Float* Float::div(STATE, Float* other) {
    return Float::create(state, this->val / other->val);
  }

  Float* Float::div(STATE, INTEGER other) {
    return Float::create(state, this->val / Float::coerce(state, other)->val);
  }

  Float* Float::mod(STATE, Float* other) {
    double res = fmod(this->val, other->val);
    if(other->val < 0.0 && this->val > 0.0 ||
       other->val > 0.0 && this->val < 0.0 ) {
      res += other->val;
    }
    return Float::create(state, res);
  }

  Float* Float::mod(STATE, INTEGER other) {
    return mod(state, Float::coerce(state, other));
  }

  Array* Float::divmod(STATE, Float* other) {
    Array* ary = Array::create(state, 2);
    ary->set(state, 0, Object::i2n((native_int) floor(this->val / other->val) ));
    ary->set(state, 1, mod(state, other));
    return ary;
  }

  Array* Float::divmod(STATE, INTEGER other) {
    return divmod(state, Float::coerce(state, other));
  }

  Float* Float::neg(STATE) {
    return Float::create(state, -this->val);
  }

  OBJECT Float::equal(STATE, Float* other) {
    if(this->val == other->val) {
      return Qtrue;
    }
    return Qfalse;
  }

  void Float::into_string(STATE, char* buf, size_t sz) {
    snprintf(buf, sz, "%+.17e", val);
  }

  void Float::Info::mark(OBJECT t, ObjectMark& mark) { }
}
