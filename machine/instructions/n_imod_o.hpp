#include "instructions.hpp"
#include "class/bignum.hpp"

/* Adapted from MRI, which includes this extensive explanation:
 *
 *  "This behaves different from C99 for negative arguments."
 */
namespace rubinius {
  namespace instructions {
    inline void n_imod_o(STATE, CF, R0, R1, R2) {
      intptr_t d = STRIP_FIXNUM_TAG(REG(r2));

      if(d == 0) {
        Exception::raise_zero_division_error(state, "divided by 0");
      }

      intptr_t n = STRIP_FIXNUM_TAG(REG(r1));

      if(n == FIXNUM_MIN && d == -1) {
        RVAL(r0) = APPLY_FIXNUM_TAG(0);
      } else {
        intptr_t m = n % d;

        if(d > 0 ? m < 0 : m > 0) {
          m += d;
        }

        if(m > FIXNUM_MAX || m < FIXNUM_MIN) {
          RVAL(r0) = Bignum::from(state, m);
        } else {
          RVAL(r0) = APPLY_FIXNUM_TAG(m);
        }
      }
    }
  }
}
