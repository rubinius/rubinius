#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_idivmod(STATE, CF, R0, R1, R2) {
      intptr_t d = STRIP_FIXNUM_TAG(REG(r2));

      if(d == 0) {
        Exception::raise_zero_division_error(state, "divided by 0");
      }

      intptr_t n = STRIP_FIXNUM_TAG(REG(r1));

      if(n == FIXNUM_MIN && d == -1) {
        RVAL(r1) = APPLY_FIXNUM_TAG(-FIXNUM_MIN);
        RVAL(r2) = APPLY_FIXNUM_TAG(0);
      } else {
        intptr_t q = n / d;
        intptr_t m = n % d;

        if(d > 0 ? m < 0 : m > 0) {
          --q;
          m += d;
        }

        if(q > FIXNUM_MAX || q < FIXNUM_MIN) {
          RVAL(r1) = Bignum::from(state, q);
        } else {
          RVAL(r1) = APPLY_FIXNUM_TAG(q);
        }

        if(m > FIXNUM_MAX || m < FIXNUM_MIN) {
          RVAL(r2) = Bignum::from(state, m);
        } else {
          RVAL(r2) = APPLY_FIXNUM_TAG(m);
        }
      }
    }
  }
}
