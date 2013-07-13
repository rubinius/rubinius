#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/encoding.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/string.hpp"
#include "builtin/tuple.hpp"
#include "configuration.hpp"
#include "missing/math.h"
#include "object_utils.hpp"
#include "ontology.hpp"
#include "util/local_buffer.hpp"
#include "version.h"

#include <gdtoa.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <sstream>
namespace rubinius {

  void Float::init(STATE) {
    GO(floatpoint).set(ontology::new_class(state, "Float", G(numeric)));
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
    Float* flt = state->new_object_dirty<Float>(G(floatpoint));
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

  Float* Float::from_cstr(STATE, const char* str, Object* strict) {
    // Skip leading whitespace and underscores.
    while(isspace(*str) || *str == '_') {
      if(*str == '_') {
        // Leading underscores are not allowed in strict mode.
        if(CBOOL(strict)) {
          return nil<Float>();
        }

        // And they return 0.0 in Ruby 1.9.
        if(!LANGUAGE_18_ENABLED) {
          return Float::create(state, 0.0);
        }
      }

      str++;
    }

    LocalBuffer b(strlen(str) + 1);
    char* buffer = (char*)b.buffer;
    char* p = buffer;
    char prev = '\0';

    while(*str) {
      // Remove underscores between digits.
      if(*str == '_') {
        if(CBOOL(strict)) {
          // Underscores are only allowed to be used as separators in strict mode.
          char next = *++str;

          if(!isdigit(prev) || !isdigit(next)) {
            return nil<Float>();
          }
        } else {
          // Else eat up all the underscores.
          while(*++str == '_');
          continue;
        }
      }

      // Ensure there is a digit to the right side of the decimal in strict mode.
      if(*str == '.' && !isdigit(str[1]) && CBOOL(strict))  {
        return nil<Float>();
      }

      prev = *str++;
      *p++ = prev;
    }

    *p = '\0';
    p = buffer;

    // Check for the hex prefix.
    if(p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
      if(CBOOL(strict)) {
        // Only allow hex in Ruby > 1.8.
        if(LANGUAGE_18_ENABLED) {
          return nil<Float>();
        }
      } else {
        // Disallow hex values when not in strict mode.
        return Float::create(state, 0.0);
      }
    }

    // Check for special values like +/-Inf[inity] and NaN.
    const char* svalue;
    bool special = true;

    // Ignore number signs.
    if(*p == '+' || *p == '-') {
      p++;
    }

    if(*p == 'I' || *p == 'i') {
      svalue = "inf";
    } else if(*p == 'N' || *p == 'n') {
      svalue = "nan";
    } else {
      special = false;
    }

    // The string just might be either Infinity or NaN.
    if(special) {
      // Case-insensitive string comparison for the special value.
      // Only check the next two characters.
      while(*++svalue && *++p) {
        if(tolower(*p) != *svalue) {
          special = false;
          break;
        }
      }

      // Disallow if the string is indeed a special value.
      if(special) {
        if(CBOOL(strict)) {
          return nil<Float>();
        } else {
          return Float::create(state, 0.0);
        }
      }
    }

    p = buffer;

    char *rest;
    double value = ruby_strtod(p, &rest);

    if(CBOOL(strict)) {
      // Disallow empty strings in strict mode.
      if(p == rest) {
        return nil<Float>();
      }

      // Only trailing spaces are allowed in strict mode.
      while(*rest) {
        if(!isspace(*rest)) {
          return nil<Float>();
        }

        rest++;
      }
    }

    return Float::create(state, value);
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

  Object* Float::fpow(STATE, Float* other) {
    if(!LANGUAGE_18_ENABLED && this->val < 0 && other->val != round(other->val)) {
      return Primitives::failure();
    }
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
    if(!LANGUAGE_18_ENABLED) {
      if(other->val == 0.0) {
        Exception::zero_division_error(state, "divided by 0");
      }
    }

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
    if(!LANGUAGE_18_ENABLED) {
      if(other->val == 0.0) {
        Exception::zero_division_error(state, "divided by 0");
      }
    }

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
    return RBOOL(this->val == other->val);
  }

  Object* Float::equal(STATE, Integer* other) {
    Float* o = Float::coerce(state, other);
    return RBOOL(this->val == o->val);
  }

  Object* Float::eql(STATE, Float* other) {
    return RBOOL(this->val == other->val);
  }

  Object* Float::eql(STATE, Integer* other) {
    return cFalse;
  }

  Object* Float::compare(STATE, Float* other) {
    if(this->val == other->val) {
      return Fixnum::from(0);
    } else if(this->val > other->val) {
      return Fixnum::from(1);
    } else if(this->val < other->val){
      return Fixnum::from(-1);
    } else {
      return cNil;
    }
  }

  Object* Float::compare(STATE, Integer* other) {
    if(isinf(this->val)) {
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
      return cNil;
    }
  }

  Object* Float::gt(STATE, Float* other) {
    return RBOOL(this->val > other->val);
  }

  Object* Float::gt(STATE, Integer* other) {
    return RBOOL(this->val > Float::coerce(state, other)->val);
  }

  Object* Float::ge(STATE, Float* other) {
    return RBOOL(this->val >= other->val);
  }

  Object* Float::ge(STATE, Integer* other) {
    return RBOOL(this->val >= Float::coerce(state, other)->val);
  }

  Object* Float::lt(STATE, Float* other) {
    return RBOOL(this->val < other->val);
  }

  Object* Float::lt(STATE, Integer* other) {
    return RBOOL(this->val < Float::coerce(state, other)->val);
  }

  Object* Float::le(STATE, Float* other) {
    return RBOOL(this->val <= other->val);
  }

  Object* Float::le(STATE, Integer* other) {
    return RBOOL(this->val <= Float::coerce(state, other)->val);
  }

  Object* Float::fisinf(STATE) {
    if(isinf(this->val) != 0) {
      return this->val < 0 ? Fixnum::from(-1) : Fixnum::from(1);
    } else {
      return cNil;
    }
  }

  Object* Float::fisnan(STATE) {
    return RBOOL(isnan(this->val));
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

    size_t size = snprintf(str, FLOAT_TO_S_STRLEN, format->c_str(state), val);

    if(size >= FLOAT_TO_S_STRLEN) {
      std::ostringstream msg;
      msg << "formatted string exceeds " << FLOAT_TO_S_STRLEN << " bytes";
      Exception::argument_error(state, msg.str().c_str());
    }

    return String::create(state, str, size);
  }

  String* Float::to_s_minimal(STATE) {
    char buffer[FLOAT_TO_S_STRLEN];

    if(g_dfmt(buffer, &val, 0, FLOAT_TO_S_STRLEN) == 0) {
      return force_as<String>(Primitives::failure());
    }

    String* str = String::create(state, buffer);
    if(is_tainted_p()) str->set_tainted();
    str->encoding(state, Encoding::usascii_encoding(state));

    return str;
  }

  Tuple* Float::dtoa(STATE) {
    int decpt, sign;
    char *s, *se;

    s = ::dtoa(val, 0, 0, &decpt, &sign, &se);

    Tuple* result = Tuple::create(state, 4);
    result->put(state, 0, String::create(state, s, se - s));
    result->put(state, 1, Fixnum::from(decpt));
    result->put(state, 2, Fixnum::from(sign));
    result->put(state, 3, Fixnum::from((int)(se - s)));

    ::freedtoa(s);

    return result;
  }

  String* Float::to_packed(STATE, Object* want_double) {
    char str[sizeof(double)];
    int sz;

    if(CBOOL(want_double)) {
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
