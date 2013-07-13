#include "builtin/bignum.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/integer.hpp"
#include "object_utils.hpp"
#include "ontology.hpp"
#include "configuration.hpp"
#include "version.h"

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

  native_int Integer::slow_to_native() {
    if(fixnum_p()) {
      return (force_as<Fixnum>(this))->to_native();
    }

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

  Integer* Integer::from_cstr(STATE, const char* str, int base,
                              Object* strict)
  {
    bool negative = false;
    Integer* value = Fixnum::from(0);

    if(base == 1 || base > 36) return nil<Integer>();

    // Skip any combination of leading whitespace and underscores.
    // Leading whitespace is OK in strict mode, but underscores are not.
    while(isspace(*str) || *str == '_') {
      if(*str == '_') {
        if(CBOOL(strict)) {
          return nil<Integer>();
        } else if(!LANGUAGE_18_ENABLED) {
          return value;
        }
      }

      str++;
    }

    if(*str == '-') {
      str++;
      negative = true;
    } else if(*str == '+') {
      str++;
    }

    char chr;
    int detected_base = 0;
    const char* str_start = str;

    // Try and detect a base prefix on the front. We have to do this
    // even though we might have been told the base, because we have
    // to know if we should discard the bytes that make up the prefix
    // if it's redundant with passed in base.
    //
    // For example, if base == 16 and str == "0xa", we return
    // to return 10. But if base == 10 and str == "0xa", we fail
    // because we rewind and try to process 0x as part of the
    // base 10 string.
    //
    if(*str == '0') {
      str++;
      switch(chr = *str++) {
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
        // If passed "017" and a base of 0, that is octal 15.
        // Otherwise, it is whatever those digits would be in the
        // specified base.
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

    // If the passed in base and the detected base contradict
    // each other, then rewind and process the whole string as
    // digits of the passed in base.
    } else if(base != detected_base) {
      // rewind the stream, and try and consume the prefix as
      // digits in the number.
      str = str_start;
    }


    bool underscore = false;

    while(*str) {
      chr = *str++;

      // If we see space characters
      if(chr == ' ' || chr == '\t' || chr == '\n' || chr == '\r') {

        // Eat them all
        while(chr == ' ' || chr == '\t' || chr == '\n' || chr == '\r') {
          chr = *str++;
        }

        // If there is more stuff after the spaces, get out of dodge.
        if(chr) {
          if(CBOOL(strict)) {
            return nil<Integer>();
          } else {
            goto return_value;
          }
        }

        break;
      }

      // If it's an underscore, remember that. An underscore is valid iff
      // it followed by a valid character for this base.
      if(chr == '_') {
        if(underscore) {
          // Double underscore is forbidden in strict mode.
          if(CBOOL(strict)) {
            return nil<Integer>();
          } else {
            // Stop parse number after two underscores in a row
            goto return_value;
          }
        }
        underscore = true;
        continue;
      } else {
        underscore = false;
      }

      // We use A-Z (and a-z) here so we support up to base 36.
      if(chr >= '0' && chr <= '9') {
        chr -= '0';
      } else if(chr >= 'A' && chr <= 'Z') {
        chr -= ('A' - 10);
      } else if(chr >= 'a' && chr <= 'z') {
        chr -= ('a' - 10);
      } else {
        //Invalid character, stopping right here.
        if(CBOOL(strict)) {
          return nil<Integer>();
        } else {
          break;
        }
      }

      // Bail if the current chr is greater or equal to the base,
      // mean it's invalid.
      if(chr >= base) {
        if(CBOOL(strict)) {
          return nil<Integer>();
        } else {
          break;
        }
      }

      if(value != Fixnum::from(0)) {
        if(Fixnum *fix = try_as<Fixnum>(value)) {
          value = fix->mul(state, Fixnum::from(base));
        } else {
          value = as<Bignum>(value)->mul(state, Fixnum::from(base));
        }
      }

      if(Fixnum *fix = try_as<Fixnum>(value)) {
        value = fix->add(state, Fixnum::from(chr));
      } else {
        value = as<Bignum>(value)->add(state, Fixnum::from(chr));
      }
    }

    // If we last saw an underscore and we're strict, bail.
    if(underscore && CBOOL(strict)) {
      return nil<Integer>();
    }

return_value:
    if(negative) {
      if(Fixnum* fix = try_as<Fixnum>(value)) {
        value = fix->neg(state);
      } else {
        value = as<Bignum>(value)->neg(state);
      }
    }

    return value;

  }

}

