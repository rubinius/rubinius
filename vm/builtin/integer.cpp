#include "builtin/integer.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/bignum.hpp"

namespace rubinius {
  
  INTEGER Integer::from(STATE, int num) {
#if (CONFIG_WORDSIZE != 64)
    if(num > FIXNUM_MAX || num < FIXNUM_MIN) {
      /* Number is too big for Fixnum. Use Bignum. */
      return Bignum::from(state, (native_int)num);
    }
#endif
    return (FIXNUM)APPLY_TAG(num, TAG_FIXNUM);
  }

  INTEGER Integer::from(STATE, unsigned int num) {
#if (CONFIG_WORDSIZE != 64)
    if(num > FIXNUM_MAX) {
      return Bignum::from(state, (unsigned long)num);
    }
#endif
    return (FIXNUM)APPLY_TAG((native_int)num, TAG_FIXNUM);
  }

  INTEGER Integer::from(STATE, unsigned long num) {
    if(num > FIXNUM_MAX) {
      return Bignum::from(state, num);
    }
    return (FIXNUM)APPLY_TAG((native_int)num, TAG_FIXNUM);
  }

  INTEGER Integer::from(STATE, long num) {
    if(num > FIXNUM_MAX || num < FIXNUM_MIN) {
      return Bignum::from(state, num);
    }
    return (FIXNUM)APPLY_TAG((native_int)num, TAG_FIXNUM);
  }

  INTEGER Integer::from(STATE, long long num) {
    if(num > FIXNUM_MAX || num < FIXNUM_MIN) {
      return Bignum::from(state, num);
    }
    return (FIXNUM)APPLY_TAG((native_int)num, TAG_FIXNUM);
  }

  INTEGER Integer::from(STATE, unsigned long long num) {
    if(num > FIXNUM_MAX) {
      return Bignum::from(state, num);
    }
    return (FIXNUM)APPLY_TAG((native_int)num, TAG_FIXNUM);
  }

}

