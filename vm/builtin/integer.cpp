#include "builtin/bignum.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/integer.hpp"
#include "object_utils.hpp"
#include "ontology.hpp"
#include "configuration.hpp"

#include <tommath.h>

#define XST ((void*)state)

namespace rubinius {

  void Numeric::init(STATE) {
    GO(numeric).set(ontology::new_class(state, "Numeric"));
    // We inherit from this type in for example Rational
    // and that class shouldn't have NumericType set since
    // that can cause problems.
    G(numeric)->set_object_type(state, ObjectType);
  }

  void Integer::init(STATE) {
    GO(integer).set(ontology::new_class(state, "Integer", G(numeric)));
    G(integer)->set_object_type(state, IntegerType);
  }

  native_int Integer::bignum_to_native() {
    return as<Bignum>(this)->to_native();
  }

  unsigned int Integer::to_uint() {
    if(fixnum_p()) {
      return (force_as<Fixnum>(this))->to_uint();
    }

    return as<Bignum>(this)->to_uint();
  }

  long Integer::to_long() {
    if(fixnum_p()) {
      return (force_as<Fixnum>(this))->to_long();
    }

    return as<Bignum>(this)->to_long();
  }

  unsigned long Integer::to_ulong() {
    if(fixnum_p()) {
      return (force_as<Fixnum>(this))->to_ulong();
    }

    return as<Bignum>(this)->to_ulong();
  }

  long long Integer::to_long_long() {
    if(fixnum_p()) {
      return (force_as<Fixnum>(this))->to_long_long();
    }

    return as<Bignum>(this)->to_long_long();
  }

  unsigned long long Integer::to_ulong_long() {
    if(fixnum_p()) {
      return (force_as<Fixnum>(this))->to_ulong_long();
    }

    return as<Bignum>(this)->to_ulong_long();
  }

  bool Integer::positive_p() {
    if(fixnum_p()) {
      return (force_as<Fixnum>(this))->positive_p();
    }

    return as<Bignum>(this)->positive_p();
  }

  Integer* Integer::from(STATE, int num) {
#ifndef IS_X8664
    if(num > FIXNUM_MAX || num < FIXNUM_MIN) {
      /* Number is too big for Fixnum. Use Bignum. */
      return Bignum::from(state, (native_int)num);
    }
#endif
    return (Fixnum*)APPLY_FIXNUM_TAG(num);
  }

  Integer* Integer::from(STATE, unsigned int num) {
#ifndef IS_X8664
    if(num > FIXNUM_MAX) {
      return Bignum::from(state, (unsigned long)num);
    }
#endif
    return (Fixnum*)APPLY_FIXNUM_TAG((native_int)num);
  }

  Integer* Integer::from(STATE, unsigned long num) {
    if(num > FIXNUM_MAX) {
      return Bignum::from(state, num);
    }
    return (Fixnum*)APPLY_FIXNUM_TAG((native_int)num);
  }

  Integer* Integer::from(STATE, long num) {
    if(num > FIXNUM_MAX || num < FIXNUM_MIN) {
      return Bignum::from(state, num);
    }
    return (Fixnum*)APPLY_FIXNUM_TAG((native_int)num);
  }

  Integer* Integer::from(STATE, long long num) {
    if(num > FIXNUM_MAX || num < FIXNUM_MIN) {
      return Bignum::from(state, num);
    }
    return (Fixnum*)APPLY_FIXNUM_TAG((native_int)num);
  }

  Integer* Integer::from(STATE, unsigned long long num) {
    if(num > FIXNUM_MAX) {
      return Bignum::from(state, num);
    }
    return (Fixnum*)APPLY_FIXNUM_TAG((native_int)num);
  }

  // Adapted from MRI.
  static const signed char digit_value[] = {
    /*     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f */
    /*0*/ -1,-1,-1,-1,-1,-1,-1,-1,-1,-2,-2,-1,-1,-2,-1,-1,
    /*1*/ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    /*2*/ -2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    /*3*/  0, 1, 2, 3, 4, 5, 6, 7, 8, 9,-1,-1,-1,-1,-1,-1,
    /*4*/ -1,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
    /*5*/ 25,26,27,28,29,30,31,32,33,34,35,-1,-1,-1,-1,'_',
    /*6*/ -1,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
    /*7*/ 25,26,27,28,29,30,31,32,33,34,35,-1,-1,-1,-1,-1,
    /*8*/ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    /*9*/ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    /*a*/ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    /*b*/ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    /*c*/ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    /*d*/ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    /*e*/ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    /*f*/ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  };

  static const int digit_bits[] = {
    1, 1, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6
  };

  Integer* Integer::from_cstr(STATE, const char* str, const char* end,
                              int base, Object* strict)
  {
    if(base == 1 || base > 36) return nil<Integer>();

    // Skip any combination of leading whitespace and underscores.  Leading
    // whitespace is OK in strict mode, but underscores are not.
    while(isspace(*str) || *str == '_') {
      if(*str == '_') {
        if(CBOOL(strict)) {
          return nil<Integer>();
        } else {
          return Fixnum::from(0);
        }
      }

      str++;
    }

    bool negative = false;

    if(*str == '-') {
      str++;
      negative = true;
    } else if(*str == '+') {
      str++;
    }

    int detected_base = 0;
    const char* str_start = str;

    /* Try to detect a base prefix. We have to do this even though we might
     * have been told the base, we have to know if we should discard the bytes
     * that make up the prefix if it's redundant with the passed in base.
     *
     * For example, if base == 16 and str == "0xa", we return 10.  But if base
     * == 10 and str == "0xa", we fail because we rewind and try to process 0x
     * as part of the base 10 string.
     */
    if(*str == '0') {
      str++;
      switch(*str++) {
      case 'b': case 'B':
        detected_base = 2;
        break;
      case 'o': case 'O':
        detected_base = 8;
        break;
      case 'd': case 'D':
        detected_base = 10;
        break;
      case 'x': case 'X':
        detected_base = 16;
        break;
      default:
        // If passed "017" and a base of 0, that is octal 15.  Otherwise, it
        // is whatever those digits would be in the specified base.
        str--;
        detected_base = 8;
      }
    }

    // If base is less than 0, then it's just a hint for how to process it
    // if there is no base detected.
    if(base < 0) {
      if(detected_base == 0) {
        // Ok, no detected because, use the base hint and start over.
        base = -base;
        str = str_start;
      } else {
        base = detected_base;
      }

    // If 0 was passed in as the base, we use the detected base.
    } else if(base == 0) {

      // Default to 10 if there is no input and no detected base.
      if(detected_base == 0) {
        base = 10;
        str = str_start;

      } else {
        base = detected_base;
      }

    // If the passed in base and the detected base contradict each other, then
    // rewind and process the whole string as digits of the passed in base.
    } else if(base != detected_base) {
      // Rewind the stream and try and consume the prefix as digits in the
      // number.
      str = str_start;
    }

    int max_digits = DIGIT_BIT / digit_bits[base];
    int count = 0;

    int digit = 0;
    mp_digit shift = base, value = 0;
    mp_int a = { 0 };

    for( ; str < end; str++) {
      digit = digit_value[int(*str)];

      if(digit >= 0 && digit < base) {
        if(++count <= max_digits) {
          value = value * base + digit;
        } else {
          if(!mp_isinitialized(&a)) {
            mp_init_set_long(XST, &a, value);
            for(int i = 0; i < max_digits - 1; i++) {
              shift *= base;
            }
          } else {
            mp_mul_d(XST, &a, shift, &a);
            mp_add_d(XST, &a, value, &a);
          }

          value = digit;
          count = 1;
        }

        continue;
      }

      // An underscore is valid iff it is followed by a valid character for
      // this base.
      if(*str == '_') {
        if(!*(str + 1) || *(str + 1) == '_') goto error_check;

        continue;
      }

      if(digit >= base) goto error_check;

      // Consume any whitespace characters.
      if(digit < -1) {
        while(digit_value[int(*++str)] < -1) /* skip whitespace */ ;

        goto error_check;
      }

      // Done parsing.
      break;
    }

error_check:

    if(str < end && CBOOL(strict)) {
      if(mp_isinitialized(&a)) mp_clear(&a);
      return nil<Integer>();
    }

    if(!mp_isinitialized(&a)) {
      if(value < FIXNUM_MAX) {
        Fixnum* result = Fixnum::from(value);
        if(negative) {
          return result->neg(state);
        } else {
          return result;
        }
      }

      mp_init_set_long(XST, &a, value);
    } else {
      shift = base;
      for(int i = 0; i < count - 1; i++) {
        shift *= base;
      }

      mp_mul_d(XST, &a, shift, &a);
      mp_add_d(XST, &a, value, &a);
    }

    if(negative) {
      mp_neg(XST, &a, &a);
    }

    Integer* result = Bignum::from(state, &a);
    mp_clear(&a);

    return result;
  }
}
