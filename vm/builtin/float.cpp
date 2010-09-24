#include "builtin/array.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/string.hpp"

#include "objectmemory.hpp"
#include "vm.hpp"
#include "vm/object_utils.hpp"
#include "primitives.hpp"

#include <gdtoa.h>

#include <cstring>
#include <cmath>
#include <iostream>
#include <sstream>

namespace rubinius {

  void Float::init(STATE) {
    GO(floatpoint).set(state->new_class("Float", G(numeric)));
    G(floatpoint)->set_object_type(state, FloatType);

    G(floatpoint)->set_const(state, "RADIX",      Fixnum::from(FLT_RADIX));
    G(floatpoint)->set_const(state, "ROUNDS",     Fixnum::from(FLT_ROUNDS));
    G(floatpoint)->set_const(state, "MIN",        Float::create(state, DBL_MIN));
    G(floatpoint)->set_const(state, "MAX",        Float::create(state, DBL_MAX));
    G(floatpoint)->set_const(state, "MIN_EXP",    Fixnum::from(DBL_MIN_EXP));
    G(floatpoint)->set_const(state, "MAX_EXP",    Fixnum::from(DBL_MAX_EXP));
    G(floatpoint)->set_const(state, "MIN_10_EXP", Fixnum::from(DBL_MIN_10_EXP));
    G(floatpoint)->set_const(state, "MAX_10_EXP", Fixnum::from(DBL_MAX_10_EXP));
    G(floatpoint)->set_const(state, "DIG",        Fixnum::from(DBL_DIG));
    G(floatpoint)->set_const(state, "MANT_DIG",   Fixnum::from(DBL_MANT_DIG));
    G(floatpoint)->set_const(state, "EPSILON",    Float::create(state, DBL_EPSILON));

  }

  Float* Float::create(STATE, double val) {
    Float* flt = state->new_struct<Float>(G(floatpoint));
    flt->val = val;
    return flt;
  }

  Float* Float::create(STATE, float val) {
    return Float::create(state, (double)val);
  }

  Float* Float::create(STATE, native_int val) {
    return Float::create(state, (double)val);
  }

  Float* Float::coerce(STATE, Object* value) {
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

  Float* Float::add(STATE, Integer* other) {
    return Float::create(state, this->val + Float::coerce(state, other)->val);
  }

  Float* Float::sub(STATE, Float* other) {
    return Float::create(state, this->val - other->val);
  }

  Float* Float::sub(STATE, Integer* other) {
    return Float::create(state, this->val - Float::coerce(state, other)->val);
  }

  Float* Float::mul(STATE, Float* other) {
    return Float::create(state, this->val * other->val);
  }

  Float* Float::mul(STATE, Integer* other) {
    return Float::create(state, this->val * Float::coerce(state, other)->val);
  }

  Float* Float::fpow(STATE, Float* other) {
    return Float::create(state, pow(this->val, other->val));
  }

  Float* Float::fpow(STATE, Integer* other) {
    return Float::create(state, pow(this->val, Float::coerce(state, other)->val));
  }

  Float* Float::div(STATE, Float* other) {
    return Float::create(state, this->val / other->val);
  }

  Float* Float::div(STATE, Integer* other) {
    return Float::create(state, this->val / Float::coerce(state, other)->val);
  }

  Float* Float::mod(STATE, Float* other) {
    double res = fmod(this->val, other->val);
    if((other->val < 0.0 && this->val > 0.0 && res != 0) ||
       (other->val > 0.0 && this->val < 0.0 && res != 0)) {
      res += other->val;
    }
    return Float::create(state, res);
  }

  Float* Float::mod(STATE, Integer* other) {
    return mod(state, Float::coerce(state, other));
  }

  Array* Float::divmod(STATE, Float* other) {
    Array* ary = Array::create(state, 2);
    ary->set(state, 0, Bignum::from_double(state, floor(this->val / other->val) ));
    ary->set(state, 1, mod(state, other));
    return ary;
  }

  Array* Float::divmod(STATE, Integer* other) {
    return divmod(state, Float::coerce(state, other));
  }

  Float* Float::neg(STATE) {
    return Float::create(state, -this->val);
  }

  Object* Float::equal(STATE, Float* other) {
    if(this->val == other->val) {
      return Qtrue;
    }
    return Qfalse;
  }

  Object* Float::equal(STATE, Integer* other) {
    Float* o = Float::coerce(state, other);
    if(this->val == o->val) {
      return Qtrue;
    }
    return Qfalse;
  }

  Object* Float::eql(STATE, Float* other) {
    if(this->val == other->val) {
      return Qtrue;
    }
    return Qfalse;
  }

  Object* Float::eql(STATE, Integer* other) {
    return Qfalse;
  }

  Object* Float::compare(STATE, Float* other) {
    if(this->val == other->val) {
      return Fixnum::from(0);
    } else if(this->val > other->val) {
      return Fixnum::from(1);
    } else if(this->val < other->val){
      return Fixnum::from(-1);
    } else {
      return Qnil;
    }
  }

  Object* Float::compare(STATE, Integer* other) {
    if(std::isinf(this->val)) {
      if(this->val > 0) {
          return Fixnum::from(1);
      } else {
        return Fixnum::from(-1);
      }
    }
    Float* o = Float::coerce(state, other);
    if(this->val == o->val) {
      return Fixnum::from(0);
    } else if(this->val > o->val) {
      return Fixnum::from(1);
    } else if(this->val < o->val){
      return Fixnum::from(-1);
    } else {
      return Qnil;
    }
  }

  Object* Float::gt(STATE, Float* other) {
    return this->val > other->val ? Qtrue : Qfalse;
  }

  Object* Float::gt(STATE, Integer* other) {
    return this->val > Float::coerce(state, other)->val ? Qtrue : Qfalse;
  }

  Object* Float::ge(STATE, Float* other) {
    return this->val >= other->val ? Qtrue : Qfalse;
  }

  Object* Float::ge(STATE, Integer* other) {
    return this->val >= Float::coerce(state, other)->val ? Qtrue : Qfalse;
  }

  Object* Float::lt(STATE, Float* other) {
    return this->val < other->val ? Qtrue : Qfalse;
  }

  Object* Float::lt(STATE, Integer* other) {
    return this->val < Float::coerce(state, other)->val ? Qtrue : Qfalse;
  }

  Object* Float::le(STATE, Float* other) {
    return this->val <= other->val ? Qtrue : Qfalse;
  }

  Object* Float::le(STATE, Integer* other) {
    return this->val <= Float::coerce(state, other)->val ? Qtrue : Qfalse;
  }

  Object* Float::fisinf(STATE) {
    if(std::isinf(this->val) != 0) {
      return this->val < 0 ? Fixnum::from(-1) : Fixnum::from(1);
    } else {
      return Qnil;
    }
  }

  Object* Float::fisnan(STATE) {
    return std::isnan(this->val) == 1 ? Qtrue : Qfalse;
  }

  Integer* Float::fround(STATE) {
    double value = this->val;
    if(value > 0.0) {
      value = floor(value);
      if(this->val - value >= 0.5) value += 1.0;
    }

    if(value < 0.0) {
      value = ceil(value);
      if(value - this->val >= 0.5) value -= 1.0;
    }
    return Bignum::from_double(state, value);
  }

  Integer* Float::to_i(STATE) {
    if(this->val > 0.0) {
      return Bignum::from_double(state, floor(this->val));
    } else if(this->val < 0.0) {
      return Bignum::from_double(state, ceil(this->val));
    }
    return Bignum::from_double(state, this->val);
  }

/* It requires "%.1022f" to print all digits of Float::MIN.
 * If you really need more digits than that, change this constant.
 */
#define FLOAT_TO_S_STRLEN   1280

  String* Float::to_s_formatted(STATE, String* format) {
    char str[FLOAT_TO_S_STRLEN];

    size_t size = snprintf(str, FLOAT_TO_S_STRLEN, format->c_str(), val);

    if(size >= FLOAT_TO_S_STRLEN) {
      std::ostringstream msg;
      msg << "formatted string exceeds " << FLOAT_TO_S_STRLEN << " bytes";
      Exception::argument_error(state, msg.str().c_str());
    }

    return String::create(state, str, size);
  }

  String* Float::to_s_minimal(STATE) {
    char str[FLOAT_TO_S_STRLEN];

    if(g_dfmt(str, &val, 0, FLOAT_TO_S_STRLEN) == 0) {
      return reinterpret_cast<String*>(kPrimitiveFailed);
    }

    return String::create(state, str);
  }

  String* Float::to_packed(STATE, Object* want_double) {
    char str[sizeof(double)];
    int sz;

    if (want_double == Qtrue) {
      double* p = (double *)str;
      *p = this->val;
      sz = 8;
    }
    else {
      float* p = (float *)str;
      *p = this->val;
      sz = 4;
    }

    return String::create(state, str, sz);
  }

  void Float::into_string(STATE, char* buf, size_t sz) {
    snprintf(buf, sz, "%+.17e", val);
  }

  void Float::Info::mark(Object* t, ObjectMark& mark) { }

  void Float::Info::show(STATE, Object* self, int level) {
    Float* f = as<Float>(self);
    std::cout << f->val << std::endl;
  }

  void Float::Info::show_simple(STATE, Object* self, int level) {
    show(state, self, level);
  }
}
