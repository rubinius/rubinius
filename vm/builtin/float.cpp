#include "builtin/float.hpp"
#include "builtin/array.hpp"
#include "builtin/fixnum.hpp"

#include "objectmemory.hpp"
#include "prelude.hpp"
#include "vm.hpp"

#include <cmath>

namespace rubinius {
  Float* Float::create(STATE, double val) {
    Float* flt = (Float*)state->new_struct(G(floatpoint), sizeof(Float));
    flt->val = val;
    return flt;
  }

  Float* Float::coerce(STATE, OBJECT value) {
    if(value->fixnum_p()) {
      return Float::create(state, (double)(as<Fixnum>(value)->to_native()));
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

  Float* Float::fpow(STATE, Float* other) {
    return Float::create(state, pow(this->val, other->val));
  }

  Float* Float::fpow(STATE, INTEGER other) {
    return Float::create(state, pow(this->val, Float::coerce(state, other)->val));
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
    ary->set(state, 0, Bignum::from_double(state, floor(this->val / other->val) ));
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

  OBJECT Float::equal(STATE, INTEGER other) {
    Float* o = Float::coerce(state, other);
    if(this->val == o->val) {
      return Qtrue;
    }
    return Qfalse;
  }

  OBJECT Float::eql(STATE, Float* other) {
    if(this->val == other->val) {
      return Qtrue;
    }
    return Qfalse;
  }

  OBJECT Float::eql(STATE, INTEGER other) {
    return Qfalse;
  }

  FIXNUM Float::compare(STATE, Float* other) {
    if(this->val == other->val) {
      return Fixnum::from(0);
    } else if(this->val > other->val) {
      return Fixnum::from(1);
    } else {
      return Fixnum::from(-1);
    }
  }

  FIXNUM Float::compare(STATE, INTEGER other) {
    Float* o = Float::coerce(state, other);
    if(this->val == o->val) {
      return Fixnum::from(0);
    } else if(this->val > o->val) {
      return Fixnum::from(1);
    } else {
      return Fixnum::from(-1);
    }
  }

  OBJECT Float::gt(STATE, Float* other) {
    return this->val > other->val ? Qtrue : Qfalse;
  }

  OBJECT Float::gt(STATE, INTEGER other) {
    return this->val > Float::coerce(state, other)->val ? Qtrue : Qfalse;
  }

  OBJECT Float::ge(STATE, Float* other) {
    return this->val >= other->val ? Qtrue : Qfalse;
  }

  OBJECT Float::ge(STATE, INTEGER other) {
    return this->val >= Float::coerce(state, other)->val ? Qtrue : Qfalse;
  }

  OBJECT Float::lt(STATE, Float* other) {
    return this->val < other->val ? Qtrue : Qfalse;
  }

  OBJECT Float::lt(STATE, INTEGER other) {
    return this->val < Float::coerce(state, other)->val ? Qtrue : Qfalse;
  }

  OBJECT Float::le(STATE, Float* other) {
    return this->val <= other->val ? Qtrue : Qfalse;
  }

  OBJECT Float::le(STATE, INTEGER other) {
    return this->val <= Float::coerce(state, other)->val ? Qtrue : Qfalse;
  }

  OBJECT Float::fisinf(STATE) {
    if(std::isinf(this->val) != 0) {
      return this->val < 0 ? Fixnum::from(-1) : Fixnum::from(1);
    } else {
      return Qnil;
    }
  }

  OBJECT Float::fisnan(STATE) {
    return std::isnan(this->val) == 1 ? Qtrue : Qfalse;
  }

  INTEGER Float::fround(STATE) {
    double value = this->val;
    if (value > 0.0) value = floor(value+0.5);
    if (value < 0.0) value = ceil(value-0.5);
    return Bignum::from_double(state, value);
  }

  INTEGER Float::to_i(STATE) {
    if(this->val > 0.0) {
      return Bignum::from_double(state, floor(this->val));
    } else if(this->val < 0.0) {
      return Bignum::from_double(state, ceil(this->val));
    }
    return Bignum::from_double(state, this->val);
  }

  void Float::into_string(STATE, char* buf, size_t sz) {
    snprintf(buf, sz, "%+.17e", val);
  }

  void Float::Info::mark(OBJECT t, ObjectMark& mark) { }
}
