#include "configuration.hpp"
#include "memory.hpp"
#include "object_utils.hpp"
#include "primitives.hpp"

#include "class/array.hpp"
#include "class/class.hpp"
#include "class/encoding.hpp"
#include "class/exception.hpp"
#include "class/fixnum.hpp"
#include "class/float.hpp"
#include "class/string.hpp"
#include "class/tuple.hpp"

#include "util/local_buffer.hpp"

#include <double-conversion.h>
#include <ieee.h>

#include <ctype.h>
#include <string.h>
#include <math.h>
#include <sstream>

#include "missing/string.h"
#include "missing/math.h"

namespace rubinius {

  void Float::bootstrap(STATE) {
    GO(floatpoint).set(state->memory()->new_class<Class, Float>(
          state, G(numeric), "Float"));

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
    Float* flt = state->memory()->new_object<Float>(state, G(floatpoint));
    flt->value(val);

    return flt;
  }

  Float* Float::create(STATE, float val) {
    return Float::create(state, (double)val);
  }

  Float* Float::create(STATE, intptr_t val) {
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

  Float* Float::from_cstr(STATE, const char* str, const char* end, Object* strict) {
    // Skip leading whitespace and underscores.
    while(str < end && (isspace(*str) || *str == '_')) {
      if(*str == '_') {
        // Leading underscores are not allowed in strict mode.
        if(CBOOL(strict)) {
          return nil<Float>();
        }

        // And they return 0.0 in Ruby 1.9.
        return Float::create(state, 0.0);
      }

      str++;
    }

    intptr_t len = end - str;

    // Skip trailing whitespace.
    if (str != end) {
      while(isspace(*(end - 1))) {
        --end;
      }
    }

    LocalBuffer b(len + 1);
    char* buffer = (char*)b.buffer;
    char* p = buffer;
    char prev = '\0';

    while(str < end) {
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
      if(!CBOOL(strict)) {
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

    char* rest;
    double value = string_to_double(buffer, strnlen(buffer, len), CBOOL(strict), &rest);

    if(CBOOL(strict)) {
      // Disallow empty strings in strict mode.
      if(buffer == rest) {
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
    return Float::create(state, this->value() + other->value());
  }

  Float* Float::add(STATE, Integer* other) {
    return Float::create(state, this->value() + Float::coerce(state, other)->value());
  }

  Float* Float::sub(STATE, Float* other) {
    return Float::create(state, this->value() - other->value());
  }

  Float* Float::sub(STATE, Integer* other) {
    return Float::create(state, this->value() - Float::coerce(state, other)->value());
  }

  Float* Float::mul(STATE, Float* other) {
    return Float::create(state, this->value() * other->value());
  }

  Float* Float::mul(STATE, Integer* other) {
    return Float::create(state, this->value() * Float::coerce(state, other)->value());
  }

  Object* Float::fpow(STATE, Float* other) {
    if(this->value() < 0 && other->value() != round(other->value())) {
      return Primitives::failure();
    }
    return Float::create(state, pow(this->value(), other->value()));
  }

  Float* Float::fpow(STATE, Integer* other) {
    return Float::create(state, pow(this->value(), Float::coerce(state, other)->value()));
  }

  Float* Float::div(STATE, Float* other) {
    return Float::create(state, this->value() / other->value());
  }

  Float* Float::div(STATE, Integer* other) {
    return Float::create(state, this->value() / Float::coerce(state, other)->value());
  }

  Float* Float::mod(STATE, Float* other) {
    if(other->value() == 0.0) {
      Exception::raise_zero_division_error(state, "divided by 0");
    }

    double res = fmod(this->value(), other->value());
    if((other->value() < 0.0 && this->value() > 0.0 && res != 0) ||
       (other->value() > 0.0 && this->value() < 0.0 && res != 0)) {
      res += other->value();
    }
    return Float::create(state, res);
  }

  Float* Float::mod(STATE, Integer* other) {
    return mod(state, Float::coerce(state, other));
  }

  Array* Float::divmod(STATE, Float* other) {
    if(other->value() == 0.0) {
      Exception::raise_zero_division_error(state, "divided by 0");
    }

    Array* ary = Array::create(state, 2);
    ary->set(state, 0, Bignum::from_double(state, floor(this->value() / other->value()) ));
    ary->set(state, 1, mod(state, other));
    return ary;
  }

  Array* Float::divmod(STATE, Integer* other) {
    return divmod(state, Float::coerce(state, other));
  }

  Float* Float::neg(STATE) {
    return Float::create(state, -this->value());
  }

  Object* Float::equal(STATE, Float* other) {
    return RBOOL(this->value() == other->value());
  }

  Object* Float::equal(STATE, Integer* other) {
    Float* o = Float::coerce(state, other);
    return RBOOL(this->value() == o->value());
  }

  Object* Float::eql(STATE, Float* other) {
    return RBOOL(this->value() == other->value());
  }

  Object* Float::eql(STATE, Integer* other) {
    return cFalse;
  }

  Object* Float::compare(STATE, Float* other) {
    if(this->value() == other->value()) {
      return Fixnum::from(0);
    } else if(this->value() > other->value()) {
      return Fixnum::from(1);
    } else if(this->value() < other->value()){
      return Fixnum::from(-1);
    } else {
      return cNil;
    }
  }

  Object* Float::compare(STATE, Integer* other) {
    if(isinf(this->value())) {
      if(this->value() > 0) {
        return Fixnum::from(1);
      } else {
        return Fixnum::from(-1);
      }
    }
    Float* o = Float::coerce(state, other);
    if(this->value() == o->value()) {
      return Fixnum::from(0);
    } else if(this->value() > o->value()) {
      return Fixnum::from(1);
    } else if(this->value() < o->value()){
      return Fixnum::from(-1);
    } else {
      return cNil;
    }
  }

  Object* Float::gt(STATE, Float* other) {
    return RBOOL(this->value() > other->value());
  }

  Object* Float::gt(STATE, Integer* other) {
    return RBOOL(this->value() > Float::coerce(state, other)->value());
  }

  Object* Float::ge(STATE, Float* other) {
    return RBOOL(this->value() >= other->value());
  }

  Object* Float::ge(STATE, Integer* other) {
    return RBOOL(this->value() >= Float::coerce(state, other)->value());
  }

  Object* Float::lt(STATE, Float* other) {
    return RBOOL(this->value() < other->value());
  }

  Object* Float::lt(STATE, Integer* other) {
    return RBOOL(this->value() < Float::coerce(state, other)->value());
  }

  Object* Float::le(STATE, Float* other) {
    return RBOOL(this->value() <= other->value());
  }

  Object* Float::le(STATE, Integer* other) {
    return RBOOL(this->value() <= Float::coerce(state, other)->value());
  }

  Object* Float::fisinf(STATE) {
    if(isinf(this->value()) != 0) {
      return this->value() < 0 ? Fixnum::from(-1) : Fixnum::from(1);
    } else {
      return cNil;
    }
  }

  Object* Float::fisnan(STATE) {
    return RBOOL(isnan(this->value()));
  }

  Integer* Float::fround(STATE) {
    double value = this->value();
    if(value > 0.0) {
      value = floor(value);
      if(this->value() - value >= 0.5) value += 1.0;
    }

    if(value < 0.0) {
      value = ceil(value);
      if(value - this->value() >= 0.5) value -= 1.0;
    }
    return Bignum::from_double(state, value);
  }

  Integer* Float::to_i(STATE) {
    if(this->value() > 0.0) {
      return Bignum::from_double(state, floor(this->value()));
    } else if(this->value() < 0.0) {
      return Bignum::from_double(state, ceil(this->value()));
    }
    return Bignum::from_double(state, this->value());
  }

/* It requires "%.1022f" to print all digits of Float::MIN.
 * If you really need more digits than that, change this constant.
 */
#define FLOAT_TO_S_STRLEN   1280

  String* Float::to_s_formatted(STATE, String* format) {
    char buf[FLOAT_TO_S_STRLEN];

    size_t size = snprintf(buf, FLOAT_TO_S_STRLEN, format->c_str(state), value());

    if(size >= FLOAT_TO_S_STRLEN) {
      std::ostringstream msg;
      msg << "formatted string exceeds " << FLOAT_TO_S_STRLEN << " bytes";
      Exception::raise_argument_error(state, msg.str().c_str());
    }
    String* str = String::create(state, buf, size);
    infect(state, str);
    str->encoding(state, Encoding::usascii_encoding(state));
    str->ascii_only(state, cTrue);
    str->valid_encoding(state, cTrue);
    return str;
  }

  String* Float::to_s_minimal(STATE) {
    char buffer[FLOAT_TO_S_STRLEN];

    int len = double_to_string(buffer, FLOAT_TO_S_STRLEN, value());
    String* str = String::create(state, buffer, len);
    infect(state, str);
    str->encoding(state, Encoding::usascii_encoding(state));
    str->ascii_only(state, cTrue);
    str->valid_encoding(state, cTrue);

    return str;
  }

  Tuple* Float::dtoa(STATE) {
    int decpt;
    bool sign;
    char buf[FLOAT_TO_S_STRLEN];

    int length = double_to_ascii(buf, FLOAT_TO_S_STRLEN, value(), &sign, &decpt);
    Tuple* result = Tuple::from(state, 4,
        String::create(state, buf, length),
        Fixnum::from(decpt),
        Fixnum::from(sign),
        Fixnum::from(length));

    return result;
  }

  String* Float::to_packed(STATE, Object* want_double) {
    char str[sizeof(double)];
    int sz;

    if(CBOOL(want_double)) {
      double* p = (double *)str;
      *p = this->value();
      sz = 8;
    }
    else {
      float* p = (float *)str;
      *p = this->value();
      sz = 4;
    }

    return String::create(state, str, sz);
  }

  Object* Float::signbit_p(STATE) {
    return signbit(this->value()) ? cTrue : cFalse;
  }

  double Float::string_to_double(const char* buf, size_t len, bool strict, char** end) {
    int flags = double_conversion::StringToDoubleConverter::ALLOW_HEX |
        double_conversion::StringToDoubleConverter::ALLOW_TRAILING_SPACES;

    if(!strict) {
      flags |= double_conversion::StringToDoubleConverter::ALLOW_TRAILING_JUNK |
        double_conversion::StringToDoubleConverter::ALLOW_LEADING_SPACES |
        double_conversion::StringToDoubleConverter::ALLOW_SPACES_AFTER_SIGN;
    }

    double_conversion::StringToDoubleConverter sd(flags, 0.0, 0.0, NULL, NULL);

    int processed;
    double value = sd.StringToDouble(buf, len, &processed);
    *end = (char*)buf + processed;

    return value;
  }

  int Float::double_to_string(char* buf, size_t len, double val) {
    double_conversion::StringBuilder builder(buf, len);
    int flags = double_conversion::DoubleToStringConverter::EMIT_TRAILING_DECIMAL_POINT |
       double_conversion::DoubleToStringConverter::EMIT_TRAILING_ZERO_AFTER_POINT |
       double_conversion::DoubleToStringConverter::EMIT_POSITIVE_EXPONENT_SIGN;
    double_conversion::DoubleToStringConverter dc(flags, "Infinity", "NaN", 'e', -4, 15, 0, 0);

    dc.ToShortest(val, &builder);

    int end = builder.position();
    builder.Finalize();
    return end;
  }

  int Float::double_to_ascii(char* buf, size_t len, double val, bool* sign, int* decpt) {
    int length;
    double_conversion::DoubleToStringConverter::DoubleToAscii(val,
      double_conversion::DoubleToStringConverter::SHORTEST, 0, buf, FLOAT_TO_S_STRLEN, sign, &length, decpt);
    return length;
  }

  void Float::into_string(STATE, char* buf, size_t sz) {
    snprintf(buf, sz, "%+.17e", value());
  }

  void Float::Info::show(STATE, Object* self, int level) {
    Float* f = as<Float>(self);
    std::cout << f->value() << std::endl;
  }

  void Float::Info::show_simple(STATE, Object* self, int level) {
    show(state, self, level);
  }
}
