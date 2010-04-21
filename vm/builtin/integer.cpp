#include "vm/object_utils.hpp"
#include "vm/config.h"

#include "builtin/integer.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/bignum.hpp"
#include "builtin/float.hpp"

namespace rubinius {
  native_int Integer::slow_to_native() {
    if(fixnum_p()) {
      return (force_as<Fixnum>(this))->to_native();
    }

    return as<Bignum>(this)->to_native();
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

}

